#include "fcfs_scheduler.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <random>
#include <sstream>
/*
int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(10, 20);

    const int num_cores = 4;
    FCFS_Scheduler scheduler(num_cores);
    scheduler.start();

    std::thread scheduler_thread;
    int process_count = 0;
    bool scheduler_testing = false;

    std::string command;
    std::string screenName;
    bool isInAnotherScreen = false;

    while (true) {
        std::cout << "Enter command: ";
        std::getline(std::cin, command);

        // Tokenize command
        std::stringstream ss(command);
        std::vector<std::string> tokens;
        std::string token;

        while (ss >> token) {
            tokens.push_back(token);
        }

        if (isInAnotherScreen && tokens.size() >= 2 && tokens[0] == "process" && tokens[1] == "-smi") {
            scheduler.print_process_details(screenName, 1);
        }
        else if (isInAnotherScreen && command == "exit") {
            isInAnotherScreen = false;
            system("cls"); // Clear screen on exit from another screen
        }
        else if (command == "screen-ls") {
            scheduler.screen_ls();
        }
        else if (command == "exit") {
            scheduler.stop();
            break;
        }
        else if (tokens.size() == 3 && tokens[0] == "screen" && tokens[1] == "-r") {
            screenName = tokens[2];
            scheduler.print_process_details(tokens[2], 0);
            isInAnotherScreen = true;
        }
        else if (command == "scheduler-test") {
            if (!scheduler_testing) {
                scheduler_testing = true;
                scheduler_thread = std::thread([&]() {
                    while (scheduler_testing) {
                        int commands_per_process = dist(gen);
                        scheduler.add_process(new Process("process" + std::to_string(++process_count), commands_per_process));
                        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait for 0.5 seconds change this to the config read the batch process freq

                    }
                    });
                scheduler_thread.detach(); // Detach the thread to run independently
            }
            else {
                std::cout << "Scheduler test is already running.\n";
            }
        }
        else if (command == "scheduler-stop") {
            if (scheduler_testing) {
                scheduler_testing = false;
                if (scheduler_thread.joinable()) {
                    scheduler_thread.join(); // Wait for scheduler thread to finish if running
                }
            }
            else {
                std::cout << "Scheduler test is not currently running.\n";
            }
        }
        else if (tokens[0] == "screen" && tokens[1] == "-s") {
            scheduler.print_process_queue_names();

        }
        else {
            std::cout << "Invalid command.\n";
        }
    }

    return 0;
}
*/