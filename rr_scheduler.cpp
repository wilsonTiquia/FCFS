#include "rr_scheduler.h"
#include <iostream>
#include <chrono>
RR_Scheduler::RR_Scheduler(int cores, int quantum) : num_cores(cores), time_quantum(quantum), running(false) {}

RR_Scheduler::~RR_Scheduler() {
    stop();
}

void RR_Scheduler::add_process(Process* proc) {
    std::lock_guard<std::mutex> lock(mtx);
    process_queue.push(proc);
    cv.notify_one();
}

void RR_Scheduler::start() {
    running = true;
    start_time = std::chrono::steady_clock::now(); // Record the start time
    for (int i = 0; i < num_cores; ++i) {
        cpu_threads.emplace_back(&RR_Scheduler::cpu_worker, this, i);
    }
    std::cout << "Scheduler started with " << num_cores << " cores.\n";
}

void RR_Scheduler::stop() {
    running = false;
    cv.notify_all();
    for (std::thread& t : cpu_threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    std::cout << "Scheduler stopped.\n";
}

void RR_Scheduler::cpu_worker(int core_id) {
    while (running) {
        Process* proc = nullptr;

        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return !process_queue.empty() || !running; });

            if (!running && process_queue.empty()) break;

            proc = process_queue.front();
            process_queue.pop();
            proc->core_id = core_id;
            proc->start_time = std::chrono::system_clock::now();
            running_processes.push_back(proc);
        }

        int remaining_commands = proc->total_commands - proc->executed_commands;
        int commands_to_execute = std::min(time_quantum, remaining_commands);
        int executed_in_quantum = 0;
        while (commands_to_execute >0 && proc->executed_commands < proc->total_commands) {
            {
                std::lock_guard<std::mutex> lock(mtx);

                proc->executed_commands+=40; // time quantum
                executed_in_quantum += 40; // time quantum
                if (proc->executed_commands >= proc->total_commands) {
                    proc->executed_commands = proc->total_commands;
                }
              //  proc->displayProcessInfo();
              
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(250)); // Simulated command execution time change this to delays per exec
           // commands_to_execute-=5;

            if (executed_in_quantum >= time_quantum) {
                break; // Exit the loop to re-add the process to the queue
            }
        }

        if (proc->executed_commands < proc->total_commands) {
            
            add_process(proc); // Re-add process if it's not completed
            std::lock_guard<std::mutex> lock(mtx);
            running_processes.remove(proc);
            
        }
        else {
            //std::cout << "Process " << proc->name << " completed.\n";
            std::lock_guard<std::mutex> lock(mtx);
            running_processes.remove(proc);
            finished_processes.push_back(proc);

        }
    }
}
void RR_Scheduler::screen_ls() {
    std::lock_guard<std::mutex> lock(mtx);
    print_running_processes();
    print_finished_processes();
}
void RR_Scheduler::print_running_processes() {
   
    std::cout << "Running processes:\n";
    for (auto& proc : running_processes) {
        std::cout << proc->name << " (" << proc->get_start_time() << ") Core: "
            << (proc->core_id == -1 ? "N/A" : std::to_string(proc->core_id))
            << " " << proc->executed_commands << " / " << proc->total_commands << "\n";
    }
    std::cout << "----------------\n";
}
void RR_Scheduler::print_finished_processes() {
    
    std::cout << "Finished processes:\n";
    for (auto& proc : finished_processes) {
        std::cout << proc->name << " (" << proc->get_start_time() << ") Finished "
            << proc->executed_commands << " / " << proc->total_commands << "\n";
    }
    std::cout << "----------------\n";
}


void RR_Scheduler::print_process_details(const std::string& process_name, int screen) {
    std::lock_guard<std::mutex> lock(mtx);

    // Check process_queue
    std::queue<Process*> temp_queue = process_queue;
    while (!temp_queue.empty()) {
        Process* proc = temp_queue.front();
        temp_queue.pop();
        if (proc->name == process_name) {
            proc->displayProcessInfo();
            return;
        }
    }

    // Check running_processes
    for (auto& proc : running_processes) {
        if (proc->name == process_name && screen == 0) {
            system("cls");
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

