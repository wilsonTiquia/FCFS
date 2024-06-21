#include "fcfs_scheduler.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>

int main() {
    const int num_processes = 10;
    const int commands_per_process = 100;
    const int num_cores = 4;

    FCFS_Scheduler scheduler(num_cores);
    scheduler.start();

    for (int i = 1; i <= num_processes; ++i) {
        scheduler.add_process(new Process("process" + std::to_string(i), commands_per_process));
    }

    std::string command;
    while (true) {
        std::cout << "Enter command: ";
        std::cin >> command;

        if (command == "screen-ls") {
            scheduler.screen_ls();
        }
        else if (command == "exit") {
            scheduler.stop();
            break;
        }
    }

    return 0;
}