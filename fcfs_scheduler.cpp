#include "fcfs_scheduler.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream> // Ensure this header is included

FCFS_Scheduler::FCFS_Scheduler(int cores) : num_cores(cores), running(true) {}

FCFS_Scheduler::~FCFS_Scheduler() {
    stop();
}

void FCFS_Scheduler::add_process(Process* proc) {
    std::lock_guard<std::mutex> lock(mtx);
    process_queue.push(proc);
    cv.notify_one();
    std::cout << "Added process " << proc->name << " to the queue.\n";
}

void FCFS_Scheduler::start() {
    for (int i = 0; i < num_cores; ++i) {
        cpu_threads.emplace_back(&FCFS_Scheduler::cpu_worker, this, i);
    }
    std::cout << "Scheduler started with " << num_cores << " cores.\n";
}

void FCFS_Scheduler::stop() {
    running = false;
    cv.notify_all();
    for (auto& thread : cpu_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    std::cout << "Scheduler stopped.\n";
}
/**/
void FCFS_Scheduler::cpu_worker(int core_id) {
    while (running) {
        Process* proc = nullptr;

        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return !process_queue.empty() || !running; });

            if (!running && process_queue.empty()) break;

            proc = process_queue.front();
            process_queue.pop();
            proc->core_id = core_id; // Assign core_id to the process
            running_processes.push_back(proc);
        }

        while (proc->executed_commands < proc->total_commands) {
            {
                std::lock_guard<std::mutex> lock(mtx);
                auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::tm local_tm;
                localtime_s(&local_tm, &now);
                proc->log_file << "(" << std::put_time(&local_tm, "%m/%d/%Y %I:%M:%S%p") << ") Core:" << core_id << " \"Hello world from " << proc->name << "!\"\n";
            }

            proc->executed_commands++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            running_processes.remove(proc);
            finished_processes.push_back(proc);
        }
    }
}
void FCFS_Scheduler::screen_ls() {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "----------------\nRunning processes:\n";

    // Print all running processes
    bool running_found = false;
    for (int i = 1; i <= 10; ++i) {
        bool found = false;

        // Check if the process is running
        for (auto& proc : running_processes) {
            if (proc->name == "process" + std::to_string(i)) {
                std::cout << proc->name << " (" << proc->get_start_time() << ") Core: " << (proc->core_id == -1 ? "N/A" : std::to_string(proc->core_id)) << " " << proc->executed_commands << " / " << proc->total_commands << "\n";
                found = true;
                running_found = true;
                break;
            }
        }

        // Check if the process is finished
        if (!found) {
            for (auto& proc : finished_processes) {
                if (proc->name == "process" + std::to_string(i)) {
                    std::cout << proc->name << " (" << proc->get_start_time() << ") Finished " << proc->total_commands << " / " << proc->total_commands << "\n";
                    found = true;
                    break;
                }
            }
        }

        // If neither running nor finished, assume not started
        if (!found) {
            std::cout << "process" + std::to_string(i) << " (N/A) N/A\n";
        }
    }

    // If no running processes were found, print a message
    if (!running_found) {
        std::cout << "No running processes.\n";
    }

    std::cout << "\nFinished processes:\n";

    // Print all finished processes
    bool finished_found = false;
    for (int i = 1; i <= 10; ++i) {
        bool found = false;
        for (auto& proc : finished_processes) {
            if (proc->name == "process" + std::to_string(i)) {
                std::cout << proc->name << " (" << proc->get_start_time() << ") Finished " << proc->total_commands << " / " << proc->total_commands << "\n";
                found = true;
                finished_found = true;
                break;
            }
        }
        
    }

    if (!finished_found) {
        std::cout << "No finished processes.\n";
    }

    std::cout << "----------------\n";
}
