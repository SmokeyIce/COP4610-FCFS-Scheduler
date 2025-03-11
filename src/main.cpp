/*
 * COP 4610 Operating Systems
 * Irina Vorobieva
 * Z23678087
 *
 * This project file simulates a FCFS Scheduler
 * Data is pulled from the process_data.txt file
 * and is used to test the functionality of the program
 *
 */

#include "fcfs/fcfs_scheduler.h"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        std::cout << "Available schedulers:" << std::endl;
        std::cout << "1. FCFS (First Come First Serve)" << std::endl;
        
        int choice;
        std::cout << "\nEnter 1 to run FCFS: ";
        std::cin >> choice;
        
        if (choice != 1) {
            std::cout << "Invalid choice. Please enter 1 for FCFS." << std::endl;
            return 1;
        }

        FCFSScheduler scheduler("process_data.txt");
        scheduler.run();
        
        std::cout << "\nPress Enter to exit...";
        std::cin.ignore();
        std::cin.get();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 