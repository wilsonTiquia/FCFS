#pragma once

#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "process.h"
#include <list>

class FCFS_Scheduler {
private:
    int num_cores;
    bool running;
    std::vector<std::thread> cpu_threads;
    std::queue<Process*> process_queue;
    std::list<Process*> running_processes;
    std::vector<Process*> finished_processes;
    std::mutex mtx;
    std::condition_variable cv;

public:
    FCFS_Scheduler(int cores);
    ~FCFS_Scheduler();

    void add_process(Process* proc);
    void start();
    void stop();
    void cpu_worker(int core_id);
    void print_running_processes();
    void print_finished_processes();
    void screen_ls();
    void print_process_details(const std::string& process_name, int screen);
    void print_process_queue_names();
};
