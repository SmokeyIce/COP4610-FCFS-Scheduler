#include "fcfs_scheduler.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <climits>

FCFSScheduler::FCFSScheduler(const std::string& filepath) : 
    filepath(filepath) {}

void FCFSScheduler::initialize_system() {
    std::cout << "Loading processes from: " << filepath << std::endl;
    processes = load_process_data(filepath);
    
    std::cout << "Initializing ready queue with " << processes.size() << " processes" << std::endl;
    for (auto& process : processes) {
        waiting_times[process.id] = 0;
        turnaround_times[process.id] = 0;
        response_times[process.id] = -1;
        ready_queue.push(&process);
    }
}

std::vector<Process> FCFSScheduler::load_process_data(const std::string& filepath) {
    std::vector<Process> processes;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filepath);
    }
    
    std::cout << "File opened successfully" << std::endl;
    
    std::string line;
    int id = 1;
    while (std::getline(file, line)) {
        std::vector<int> bursts;
        std::istringstream iss(line);
        int burst;
        while (iss >> burst) {
            bursts.push_back(burst);
        }
        
        if (!bursts.empty()) {
            processes.emplace_back(id++, bursts);
            std::cout << "Process " << processes.back().id << " bursts:";
            for (int b : bursts) {
                std::cout << " " << b;
            }
            std::cout << std::endl;
        }
    }
    
    return processes;
}

void FCFSScheduler::run_next_process() {
    current_process->set_running();
    
    int cpu_burst_time = current_process->next_CPU_burst();
    std::cout << "Process P" << current_process->id 
              << " starting CPU burst of " << cpu_burst_time 
              << " at time " << current_time << std::endl;
    
    // Execute CPU burst
    cpu_busy_time += cpu_burst_time;
    current_time += cpu_burst_time;
    current_process->completion_time = current_time;
    current_process->current_burst++;
    
    if (current_process->has_more_bursts()) {
        current_process->set_waiting();
        waiting_queue.push_back(current_process);
    } else {
        complete_process(current_process);
    }
    
    current_process = nullptr;
}

void FCFSScheduler::check_IO_completions() {
    std::vector<Process*> completed_io;
    
    for (auto* process : waiting_queue) {
        int io_burst = process->bursts[process->current_burst];
        if (current_time >= process->completion_time + io_burst) {
            process->current_burst++;
            process->set_ready();
            ready_queue.push(process);
            completed_io.push_back(process);
        }
    }
    
    for (auto* process : completed_io) {
        waiting_queue.erase(
            std::remove(waiting_queue.begin(), waiting_queue.end(), process),
            waiting_queue.end()
        );
    }
}

void FCFSScheduler::update_process_times() {
    std::queue<Process*> temp_queue = ready_queue;
    std::vector<Process*> processes_in_order;
    
    // First, get processes in order
    while (!temp_queue.empty()) {
        processes_in_order.push_back(temp_queue.front());
        temp_queue.pop();
    }
    
    // Calculate waiting time for each process based on CPU bursts ahead of it
    for (size_t i = 0; i < processes_in_order.size(); i++) {
        int wait_for_this_process = 0;
        
        // Sum up CPU bursts of all processes ahead in queue
        for (size_t j = 0; j < i; j++) {
            // Only consider the next CPU burst for each process ahead
            if (processes_in_order[j]->current_burst < processes_in_order[j]->bursts.size()) {
                // Since bursts alternate CPU-IO, only even indices are CPU bursts
                if (processes_in_order[j]->current_burst % 2 == 0) {
                    wait_for_this_process += processes_in_order[j]->bursts[processes_in_order[j]->current_burst];
                }
            }
        }
        
        // Add to (not replace) current process's waiting time when it's in ready queue
        waiting_times[processes_in_order[i]->id] += wait_for_this_process;
    }
    
    // Also consider the current running process's remaining CPU burst
    if (current_process != nullptr && 
        current_process->current_burst < current_process->bursts.size() &&
        current_process->current_burst % 2 == 0) {
        int remaining_burst = current_process->bursts[current_process->current_burst];
        for (auto* process : processes_in_order) {
            waiting_times[process->id] += remaining_burst;
        }
    }
}

void FCFSScheduler::complete_process(Process* process) {
    process->set_completed();
    process->completion_time = current_time;
    
    // Turnaround time = completion time - arrival time
    turnaround_times[process->id] = current_time - process->arrival_time;
    
    completed_processes.push_back(process);
}

bool FCFSScheduler::CPU_is_idle() const {
    return current_process == nullptr;
}

bool FCFSScheduler::all_processes_completed() const {
    return completed_processes.size() == processes.size() &&
           ready_queue.empty() &&
           waiting_queue.empty() &&
           current_process == nullptr;
}

void FCFSScheduler::print_system_state() const {
    std::cout << "\nTime: " << current_time << std::endl;
    
    std::cout << "Running: ";
    if (current_process != nullptr) {
        std::cout << "P" << current_process->id;
    }
    std::cout << std::endl;
    
    std::cout << "Ready: ";
    std::queue<Process*> temp_queue = ready_queue;
    while (!temp_queue.empty()) {
        std::cout << "P" << temp_queue.front()->id << " ";
        temp_queue.pop();
    }
    std::cout << std::endl;
    
    std::cout << "I/O: ";
    for (const auto* process : waiting_queue) {
        std::cout << "P" << process->id << " ";
    }
    std::cout << std::endl;
}

void FCFSScheduler::print_final_metrics() const {
    double avg_waiting = 0;
    double avg_turnaround = 0;
    double avg_response = 0;
    
    std::cout << "\nProcess Metrics:" << std::endl;
    std::cout << "     Tw    Ttr    Tr" << std::endl;
    for (const auto& process : processes) {
        std::cout << "P" << process.id << "  " 
                  << std::setw(5) << waiting_times.at(process.id) << " "
                  << std::setw(6) << turnaround_times.at(process.id) << " "
                  << std::setw(5) << response_times.at(process.id) << std::endl;
        
        avg_waiting += waiting_times.at(process.id);
        avg_turnaround += turnaround_times.at(process.id);
        avg_response += response_times.at(process.id);
    }
    
    int n = processes.size();
    std::cout << "\nFinal Metrics:" << std::endl;
    std::cout << "CPU Utilization: " << std::fixed << std::setprecision(2)
              << (cpu_busy_time * 100.0 / current_time) << "%" << std::endl;
    std::cout << "Average Waiting Time: " << std::fixed << std::setprecision(2)
              << (avg_waiting / n) << std::endl;
    std::cout << "Average Turnaround Time: " << (avg_turnaround / n) << std::endl;
    std::cout << "Average Response Time: " << (avg_response / n) << std::endl;
}

void FCFSScheduler::run() {
    initialize_system();
    
    while (!all_processes_completed()) {
        std::cout << "\n=== Time step: " << current_time << " ===" << std::endl;
        
        bool did_work = false;
        
        // Check I/O completions first
        check_IO_completions();
        
        // Start CPU burst if CPU is idle and ready queue not empty
        if (CPU_is_idle() && !ready_queue.empty()) {
            current_process = ready_queue.front();
            ready_queue.pop();
            
            // Track response time when process first starts
            if (response_times[current_process->id] == -1) {
                response_times[current_process->id] = current_time;
            }
            
            run_next_process();
            did_work = true;
        }

        // Update waiting times for processes in ready queue
        update_process_times();
        
        print_system_state();
        
        // Only increment time if no work was done
        if (!did_work && !waiting_queue.empty()) {
            // Find next I/O completion time
            int next_completion = INT_MAX;
            for (auto* process : waiting_queue) {
                int completion_time = process->completion_time + process->bursts[process->current_burst];
                next_completion = std::min(next_completion, completion_time);
            }
            current_time = next_completion;
        }
    }
    
    print_final_metrics();
} 