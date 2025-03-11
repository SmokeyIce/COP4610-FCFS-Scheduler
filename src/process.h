#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <string>

class Process {
public:
    enum State {
        NEW,
        READY,
        RUNNING,
        WAITING,
        COMPLETED
    };

    int id;
    State state;
    std::vector<int> bursts;
    size_t current_burst;
    int arrival_time;
    int completion_time;
    int waiting_time;     // Add explicit waiting time tracking
    int turnaround_time;
    int response_time;    // Add response time tracking

    Process(int id, const std::vector<int>& bursts, int arrival_time = 0)
        : id(id), 
          state(NEW),
          bursts(bursts),
          current_burst(0),
          arrival_time(arrival_time),
          completion_time(0),
          waiting_time(0),
          turnaround_time(0),
          response_time(-1) {}

    bool has_more_bursts() const {
        return current_burst < bursts.size();
    }

    int next_CPU_burst() const {
        return current_burst < bursts.size() ? bursts[current_burst] : 0;
    }

    // Add helper methods for state transitions
    void set_ready() { state = READY; }
    void set_running() { state = RUNNING; }
    void set_waiting() { state = WAITING; }
    void set_completed() { state = COMPLETED; }
    
    bool is_waiting() const { return state == WAITING; }
    bool is_completed() const { return state == COMPLETED; }
};

#endif 