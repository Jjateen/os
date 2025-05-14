#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 10
#define MAX_LINE_LENGTH 100
#define QUANTUM 5

typedef struct {
    char pid[5];
    int arrival_time;
    int cpu_burst;
    int total_cpu_burst;
    int io_units;
    int cpu_units_per_io;
    int current_cpu_count;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    int remaining_cpu_burst;
    int in_io;
    int io_finish_time;
    int started;
    int time_in_current_quantum;
    int last_cpu_time; // For accurate waiting time calculation
} Process;

// Function to read processes from file
int read_processes_from_file(const char* filename, Process processes[]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return 0;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;

    while (fgets(line, MAX_LINE_LENGTH, file) && count < MAX_PROCESSES) {
        char *token;
        token = strtok(line, ";");
        strcpy(processes[count].pid, token);

        token = strtok(NULL, ";");
        processes[count].arrival_time = atoi(token);

        token = strtok(NULL, ";");
        processes[count].cpu_burst = atoi(token);
        processes[count].total_cpu_burst = processes[count].cpu_burst;
        processes[count].remaining_cpu_burst = processes[count].cpu_burst;

        token = strtok(NULL, ";");
        processes[count].io_units = atoi(token);

        token = strtok(NULL, ";");
        processes[count].cpu_units_per_io = atoi(token);

        // Initialize other fields
        processes[count].current_cpu_count = 0;
        processes[count].completion_time = 0;
        processes[count].waiting_time = 0;
        processes[count].turnaround_time = 0;
        processes[count].response_time = -1; // -1 means not yet started
        processes[count].in_io = 0;
        processes[count].io_finish_time = 0;
        processes[count].started = 0;
        processes[count].time_in_current_quantum = 0;
        processes[count].last_cpu_time = -1;

        count++;
    }

    fclose(file);
    return count;
}

// Function to implement Round Robin scheduling
void round_robin_scheduler(const char* filename) {
    Process processes[MAX_PROCESSES];
    int n = read_processes_from_file(filename, processes);
    
    if (n == 0) {
        printf("No processes read from file.\n");
        return;
    }

    int current_time = 0;
    int completed_processes = 0;
    
    // Ready queue implementation
    int ready_queue[MAX_PROCESSES * 100]; // Large enough to handle multiple reentries
    int front = 0;
    int rear = -1;
    int queue_size = 0;
    
    // I/O queue and tracking
    int io_queue[MAX_PROCESSES];
    int io_queue_count = 0;
    int io_busy_until = 0;
    int current_io_process = -1;
    
    // For tracking current process and execution timeline
    int current_process = -1;
    char timeline[1000] = {0};
    int timeline_len = 0;

    // Main scheduling loop
    while (completed_processes < n) {
        // Check for new arrivals
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time == current_time && 
                processes[i].remaining_cpu_burst > 0 && 
                !processes[i].in_io) {
                // Add to ready queue
                rear = (rear + 1) % (MAX_PROCESSES * 100);
                ready_queue[rear] = i;
                queue_size++;
            }
        }
        
        // Check if I/O is complete for any process
        if (current_io_process != -1 && io_busy_until <= current_time) {
            processes[current_io_process].in_io = 0;
            
            // Add to ready queue if it has remaining CPU burst
            if (processes[current_io_process].remaining_cpu_burst > 0) {
                rear = (rear + 1) % (MAX_PROCESSES * 100);
                ready_queue[rear] = current_io_process;
                queue_size++;
            }
            
            current_io_process = -1;
        }

        // Start I/O for a process if the I/O device is free
        if (current_io_process == -1 && io_queue_count > 0) {
            current_io_process = io_queue[0];
            
            // Shift the I/O queue
            for (int i = 0; i < io_queue_count - 1; i++) {
                io_queue[i] = io_queue[i + 1];
            }
            io_queue_count--;
            
            io_busy_until = current_time + processes[current_io_process].io_units;
        }

        // If current process has completed its quantum or needs I/O or is done
        if (current_process != -1) {
            if (processes[current_process].time_in_current_quantum >= QUANTUM || 
                processes[current_process].remaining_cpu_burst == 0 ||
                processes[current_process].current_cpu_count % processes[current_process].cpu_units_per_io == 0) {
                
                // Reset quantum counter
                processes[current_process].time_in_current_quantum = 0;
                
                // If process needs I/O
                if (processes[current_process].current_cpu_count % processes[current_process].cpu_units_per_io == 0 && 
                    processes[current_process].remaining_cpu_burst > 0) {
                    
                    processes[current_process].in_io = 1;
                    io_queue[io_queue_count++] = current_process;
                    processes[current_process].last_cpu_time = current_time;
                }
                // If process has remaining CPU burst and didn't need I/O, put it back in the ready queue
                else if (processes[current_process].remaining_cpu_burst > 0) {
                    rear = (rear + 1) % (MAX_PROCESSES * 100);
                    ready_queue[rear] = current_process;
                    queue_size++;
                    processes[current_process].last_cpu_time = current_time;
                }
                // If process is completed
                else if (processes[current_process].remaining_cpu_burst == 0) {
                    processes[current_process].completion_time = current_time;
                    processes[current_process].turnaround_time = processes[current_process].completion_time - processes[current_process].arrival_time;
                    
                    // Calculate waiting time: turnaround time - total CPU burst
                    processes[current_process].waiting_time = processes[current_process].turnaround_time - processes[current_process].total_cpu_burst;
                    
                    completed_processes++;
                }
                
                current_process = -1;
            }
        }
        
        // If no current process, get the next one from ready queue
        if (current_process == -1 && queue_size > 0) {
            current_process = ready_queue[front];
            front = (front + 1) % (MAX_PROCESSES * 100);
            queue_size--;
            
            // If this is the first time the process is running, record response time
            if (!processes[current_process].started) {
                processes[current_process].response_time = current_time - processes[current_process].arrival_time;
                processes[current_process].started = 1;
            }
        }

        // Execute current process for this time unit
        if (current_process != -1) {
            // Add to timeline
            if (timeline_len < 999) {
                timeline[timeline_len++] = processes[current_process].pid[1]; // Using second character of PID for timeline
            }
            
            processes[current_process].current_cpu_count++;
            processes[current_process].remaining_cpu_burst--;
            processes[current_process].time_in_current_quantum++;
        } else {
            // CPU idle for this time unit
            if (timeline_len < 999) {
                timeline[timeline_len++] = '-';
            }
        }

        // Check for processes that have completed I/O or arrived since the beginning of this time unit
        for (int i = 0; i < n; i++) {
            // If a process has just arrived and wasn't already accounted for
            if (processes[i].arrival_time == current_time + 1 && 
                processes[i].remaining_cpu_burst > 0 && 
                !processes[i].in_io) {
                // It will be picked up at the start of the next time unit
            }
        }
        
        current_time++;
    }
    
    // Null-terminate timeline
    timeline[timeline_len] = '\0';
    
    // Print performance metrics in the requested table format
    printf("Process    Arrival    Burst    Completion   Turnaround   Waiting    Response\n");
    
    for (int i = 0; i < n; i++) {
        printf("%-10s %-10d %-8d %-13d %-12d %-10d %-8d\n", 
               processes[i].pid, 
               processes[i].arrival_time, 
               processes[i].total_cpu_burst,
               processes[i].completion_time,
               processes[i].turnaround_time,
               processes[i].waiting_time,
               processes[i].response_time);
    }
    
    // Calculate and print averages and throughput
    double total_turnaround = 0;
    double total_waiting = 0;
    double total_response = 0;
    int max_completion = 0;
    
    for (int i = 0; i < n; i++) {
        total_turnaround += processes[i].turnaround_time;
        total_waiting += processes[i].waiting_time;
        total_response += processes[i].response_time;
        
        if (processes[i].completion_time > max_completion) {
            max_completion = processes[i].completion_time;
        }
    }
    
    printf("\nAverage Turnaround Time: %.2f\n", total_turnaround / n);
    printf("Average Waiting Time: %.2f\n", total_waiting / n);
    printf("Average Response Time: %.2f\n", total_response / n);
    printf("System Throughput: %.5f processes per unit time\n", (double)n / max_completion);
    
    // Print timeline
    printf("\nExecution Timeline (- = idle, 0,1,2,3 = process number):\n%s\n", timeline);
}

int main() {
    const char* filename = "input.txt";
    round_robin_scheduler(filename);
    return 0;
}