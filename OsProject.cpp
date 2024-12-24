#include <iostream>     
#include <vector>       
#include <fstream>      
#include <string>       
#include <queue>        
#include <sstream>
using namespace std;

// Process structure to store all information about a process
struct Process {
    string name;          
    int arrival_time;     
    int burst_time;      
    int start_time;      
    int finish_time;     
};

void read_input(string filename, vector<Process>& processes, int& num_processes, int& time_quantum) {
    ifstream file(filename);
    string line;
    
    while (getline(file, line)) {
        char type = line[0];  // Get first character
                                          /*  N 3
                                           A 0 2 4
                                           B 3 6 4
                                           Q 2  */
        if (type == 'N') {
            // If line is "N 3"
            stringstream ss(line.substr(2));  
            ss >> num_processes;           // 3
            
            // Create P1, P2, P3
            for (int i = 0; i < num_processes; i++) {
                Process p;
                p.name = "P" + to_string(i + 1);
                processes.push_back(p);
            }
        }
        else if (type == 'A') {
            // Get arrival times
            string arr_times = line.substr(2);
            stringstream ss(arr_times); // 0 2 4 
            int time, i = 0;
            while (ss >> time) {
                processes[i++].arrival_time = time;
            }
        }
        else if (type == 'B') {
            // Get burst times
            
            string burst_times = line.substr(2); 
            stringstream ss(burst_times); // 3 6 4
            int time, i = 0;
            while (ss >> time) {
                processes[i++].burst_time = time;
            }
        }
        else if (type == 'Q') {
            
        stringstream ss(line.substr(2));
         ss >> time_quantum;        }
    }
    file.close();
}


vector<string> get_fcfs_timeline(vector<Process> processes) {
    vector<string> timeline;
    int current_time = 0;
    
    for (int i = 0; i < processes.size(); i++) {
        // Add idle time if there's a gap between processes
        while (current_time < processes[i].arrival_time) {
            timeline.push_back("Idle");
            current_time++;
        }
        // Execute current process 
        for (int j = 0; j < processes[i].burst_time; j++) {
            timeline.push_back(processes[i].name);
            current_time++;
        }
    }
    return timeline;
}


vector<string> get_round_robin_timeline(vector<Process> processes, int time_quantum) {
    vector<string> timeline;
    queue<int> ready_queue;        
    vector<int> remaining_time;    
    
    // Remaining is the burst time of each process
    for (int i = 0; i < processes.size(); i++) {
        remaining_time.push_back(processes[i].burst_time);
    }
    
    int current_time = 0;
    int completed = 0;    // Number of completed processes
    int i = 0;           
    
    
// N 3        
// A 0  2 4    
// B 3 6 4    
// Q 2
    
    while (completed < processes.size()) {
        //  add  processes to ready queue
        while (i < processes.size() && processes[i].arrival_time <= current_time) {
            ready_queue.push(i);
            i++;
        }
        
        // If no process , then its idle
        if (ready_queue.empty()) {
            timeline.push_back("Idle");
            current_time++;
            continue;
        }
        
        // Bring the first process in the queue
        int current_process = ready_queue.front();
        ready_queue.pop();
        
        // Execute 
        for (int j = 0; j < time_quantum && remaining_time[current_process] > 0; j++) {
            timeline.push_back(processes[current_process].name);
            remaining_time[current_process]--;
            current_time++;
            
            while (i < processes.size() && processes[i].arrival_time <= current_time) {
                ready_queue.push(i);
                i++;
            }
        }
        
        
        if (remaining_time[current_process] > 0) {
            ready_queue.push(current_process);
        } else {
            completed++;  
        }
    }
    return timeline;
}


vector<string> get_srt_timeline(vector<Process> processes) {
    vector<string> timeline;
    vector<int> remaining_time;    
    int current_time = 0;
    int completed = 0;            
    
    // store the burst time 
    for (int i = 0; i < processes.size(); i++) {
        remaining_time.push_back(processes[i].burst_time);
    }
    
    // Continue until all are done
    while (completed < processes.size()) {
        int shortest = -1;        
        int min_time = INT_MAX;    
        
        // Find the shortest process
        for (int i = 0; i < processes.size(); i++) {
            if (processes[i].arrival_time <= current_time && // Process is here
                remaining_time[i] > 0 &&                     // Process still didn't finish
                remaining_time[i] < min_time) {              // if the process has the shortest time
                min_time = remaining_time[i];
                shortest = i;
            }
        }
        
        // If no process available, add idle time
        if (shortest == -1) {
            timeline.push_back("Idle");
            current_time++;
        }
        // Execute 
        else {
            timeline.push_back(processes[shortest].name);
            remaining_time[shortest]--;
            current_time++;
            
            // Check if process has completed
            if (remaining_time[shortest] == 0) {
                completed++;
            }
        }
    }
    return timeline;
}

void S_AND_F(vector<Process>& processes, vector<string>& timeline) {
    //timeline = ["P1", "P1", "P2", "P2", "P3", "P3", "P1"]

    for (int i = 0; i < processes.size(); i++) {
        // Find first 
        for (int j = 0; j < timeline.size(); j++) {
            if (timeline[j] == processes[i].name) {
                processes[i].start_time = j;
                break;
            }
        }
        // Find last 
        for (int j = timeline.size() - 1; j >= 0; j--) {
            if (timeline[j] == processes[i].name) {
                processes[i].finish_time = j + 1;
                break;
            }
        }
    }
}

// Print  results for each process
void print_results(vector<Process>& processes) {
    cout << "\nProcess Results:" << endl;
    for (int i = 0; i < processes.size(); i++) {
        cout << processes[i].name << ":" << endl;
        cout << "Start Time: " << processes[i].start_time << endl;
        cout << "Finish Time: " << processes[i].finish_time << endl;

        // Waiting time = Start time - Arrival time
        cout << "Waiting Time: " << processes[i].start_time - processes[i].arrival_time << endl;

        // Turnaround time = Finish time - Arrival time
        cout << "Turnaround Time: " << processes[i].finish_time - processes[i].arrival_time << endl;
        cout << endl;
    }
}


void cpu_utilization(vector<Process>& processes) {
    int total_burst_time = 0;
    int finish_time = 0;
    
    // Calculate total burst time and find latest finish time
    for (int i = 0; i < processes.size(); i++) {
        total_burst_time += processes[i].burst_time;
        if (processes[i].finish_time > finish_time) {
            finish_time = processes[i].finish_time;
        }
    }
    
    // CPU utilization = (Total burst time / Total time) * 100
    double cpu_utilization = (double)total_burst_time / finish_time * 100;
    cout << "CPU Utilization: " << cpu_utilization << "%" << endl;
}

// Print Gantt chart showing process execution timeline
void gantt_chart(const vector<string>& timeline) {
    cout << "Gantt Chart:\n";
    int i = 0;
    // Print timeline with time markers
    for (i = 0; i < timeline.size(); i++) {
        cout << i << "   " << timeline[i] << "   ";

       // timeline = ["P1", "P1", "P2", "P2", "P3", "P3", "P1"]

        // Skip duplicate consecutive entries
        for (int j = i; j < timeline.size(); j++) {
            if (j + 1 < timeline.size() && timeline[j] == timeline[j + 1])
                i++;
            else break;
        }
    }
    cout << i << "\n\n";
}

int main() {
    vector<Process> processes;
    int num_processes;
    int time_quantum;
    
    read_input("processes.txt", processes, num_processes, time_quantum);
    
    cout << "\n=== First Come First Serve ===" << endl;
    vector<string> fcfs_timeline = get_fcfs_timeline(processes);
    gantt_chart(fcfs_timeline);
    S_AND_F(processes, fcfs_timeline);
    print_results(processes);
    cpu_utilization(processes);
    
    cout << "\n=== Round Robin ===" << endl;
    vector<string> rr_timeline = get_round_robin_timeline(processes, time_quantum);
    gantt_chart(rr_timeline);
    S_AND_F(processes, rr_timeline);
    print_results(processes);
    cpu_utilization(processes);
    
    cout << "\n=== Shortest Remaining Time ===" << endl;
    vector<string> srt_timeline = get_srt_timeline(processes);
    gantt_chart(srt_timeline);
    S_AND_F(processes, srt_timeline);
    print_results(processes);
    cpu_utilization(processes);
    
    return 0;
}