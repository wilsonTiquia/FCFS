#include "sjf_scheduler.h"
#include <iostream>
#include <random>

SJF_Scheduler::SJF_Scheduler(int cores) : num_cores(cores), running(true) {}

SJF_Scheduler::~SJF_Scheduler() {}


void SJF_Scheduler::add_process(Process* proc) {
    std::lock_guard<std::mutex> lock(mtx);
    process_queue.push(proc);
    cv.notify_one();

}
void SJF_Scheduler::start() {
    for (int i = 0; i < num_cores; ++i) {
        cpu_threads.emplace_back(&SJF_Scheduler::cpu_worker, this, i);
    }

    std::cout << "Scheduler started with " << num_cores << " cores.\n";
}
void SJF_Scheduler::stop() {
    running = false;
    cv.notify_all();
    for (auto& thread : cpu_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    std::cout << "Scheduler stopped.\n";
}

void SJF_Scheduler::cpu_worker(int core_id) {
    // Set up the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> exec_dist(1, 10); // Random number of commands to execute per iteration change this to the config min ins and max ins

    while (running) {
        Process* proc = nullptr;

        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return !process_queue.empty() || !running; });

            if (!running && process_queue.empty()) break;

            proc = process_queue.top();
            process_queue.pop();
            proc->core_id = core_id; // Assign core_id to the process
            proc->start_time = std::chrono::system_clock::now();
            running_processes.push_back(proc);
        
        }

        while (proc->executed_commands < proc->total_commands) {
            {
                auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::tm local_tm;
                localtime_s(&local_tm, &now);
                proc->log_file << "(" << std::put_time(&local_tm, "%m/%d/%Y %I:%M:%S%p") << ") Core:" << core_id << " \"Hello world from " << proc->name << "!\"\n";

                std::lock_guard<std::mutex> lock(mtx);
                proc->executed_commands += exec_dist(gen); // Random number of commands executed
              
                if (proc->executed_commands > proc->total_commands) {
                    proc->executed_commands = proc->total_commands; // Ensure we don't exceed total_commands
                }
            }

            // Sleep for a defined duration (e.g., 250 milliseconds)
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            running_processes.remove(proc);
            finished_processes.push_back(proc);
        }
    }
}

void SJF_Scheduler::print_running_processes() {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Running processes:\n";
    for (auto& proc : running_processes) {
        std::cout << proc->name << " (" << proc->get_start_time() << ") Core: "
            << (proc->core_id == -1 ? "N/A" : std::to_string(proc->core_id))
            << " " << proc->executed_commands << " / " << proc->total_commands << "\n";
    }
    std::cout << "----------------\n";
}

void SJF_Scheduler::print_finished_processes() {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Finished processes:\n";
    for (auto& proc : finished_processes) {
        std::cout << proc->name << " (" << proc->get_start_time() << ") Finished "
            << proc->executed_commands << " / " << proc->total_commands << "\n";
    }
    std::cout << "----------------\n";
}

void SJF_Scheduler::screen_ls() {
    print_running_processes();
    print_finished_processes();
}

void SJF_Scheduler::print_process_details(const std::string& process_name, int screen) {
    std::lock_guard<std::mutex> lock(mtx);

    // Check process_queue
    std::priority_queue<Process*, std::vector<Process*>, CompareProcess> temp_queue = process_queue;
    while (!temp_queue.empty()) {
        Process* proc = temp_queue.top();
        temp_queue.pop();
        if (proc->name == process_name) {
            proc->displayProcessInfo();
            return;
        }
    }

    // Check running_processes
    for (auto& proc : running_processes) {
        if (proc->name == process_name && screen == 0) {
            system("cls"); // Clear screen for better readability
            proc->displayProcessInfo();
            return;
        }
        else if (proc->name == process_name && screen == 1) {
            proc->displayProcessInfo();
            return;
        }
    }

    // Check finished_processes
    for (auto& proc : finished_processes) {
        if (proc->name == process_name && screen == 1) {
            proc->displayProcessInfo();
            std::cout << "Process " << process_name << " has finished and cannot be accessed after exiting this screen.\n";
            return;
        }
        else if (proc->name == process_name && screen == 0) {
            std::cout << "Process " << process_name << " not found.\n";
            return;
        }
    }

    // If process not found in any list
    std::cout << "Process " << process_name << " not found.\n";
}