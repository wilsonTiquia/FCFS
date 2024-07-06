#include "sjf_scheduler.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <random>

std::atomic<bool> keep_running(true); // To control the loop
std::atomic<bool> generate_processes(false); // To control process generation
std::list<std::string> generated;
void add_processes(SJF_Scheduler& scheduler, int& process_counter) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(10, 100);

    while (keep_running) {
        if (generate_processes) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            int burst_time = dist(gen);
            std::string process_name = "process" + std::to_string(++process_counter);
            scheduler.add_process(new Process(process_name, burst_time));

          //  std::cout << "Added: " << process_name << " with burst time: " << burst_time << std::endl;
            generated.push_back(process_name + " " + std::to_string(burst_time));
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Small sleep to avoid busy waiting
        }
    }
}

int main() {
    const int num_cores = 1; // change this to the number of corse
    SJF_Scheduler scheduler(num_cores);
    scheduler.start();

    int process_counter = 0; // Start with process0
 



    std::string command;
    std::string screenName;
    bool isInAnotherScreen = false;

    std::thread process_thread(add_processes, std::ref(scheduler), std::ref(process_counter));

    // Start a thread to add processes periodically
    // Main command processing loop
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
            system("cls");
            screenName = tokens[2];
            scheduler.print_process_details(tokens[2], 0);
            isInAnotherScreen = true;
        }
        else if (command == "scheduler-start") {
            generate_processes = true;
            std::cout << "Process generation started." << std::endl;
        }
        else if (command == "scheduler-stop") {
            generate_processes = false;
            std::cout << "Process generation stopped." << std::endl;
        }


    }

    // Join the add_process_thread to safely terminate the program
   

    return 0;
}