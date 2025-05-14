#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 10
#define MAX_LINE_LENGTH 100

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
    int last_execution_time; // Track when process was last executed for accurate waiting time
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
        processes[count].last_execution_time = -1; // Not executed yet

        count++;
    }

    fclose(file);
    return count;
}

// Function to find the process with shortest remaining burst time that is ready to run
int find_shortest_remaining_time(Process processes[], int n, int current_time) {
    int shortest_job_index = -1;
    int shortest_burst = 999999;

    for (int i = 0; i < n; i++) {
        // Process is ready if it has arrived, has remaining burst time, and isn't in I/O
        if (processes[i].arrival_time <= current_time && 
            processes[i].remaining_cpu_burst > 0 && 
            !processes[i].in_io) {
            
            if (processes[i].remaining_cpu_burst < shortest_burst) {
                shortest_burst = processes[i].remaining_cpu_burst;
                shortest_job_index = i;
            }
        }
    }

    return shortest_job_index;
}

// Function to implement SRTF scheduling
void srtf_scheduler(const char* filename) {
    Process processes[MAX_PROCESSES];
    int n = read_processes_from_file(filename, processes);
    
    if (n == 0) {
        printf("No processes read from file.\n");
        return;
    }

    int current_time = 0;
    int completed_processes = 0;
    int current_process = -1;
    int io_queue[MAX_PROCESSES];
    int io_queue_count = 0;
    int io_busy_until = 0;
    int current_io_process = -1;
    
    // For logging/debug
    char timeline[1000] = {0};
    int timeline_len = 0;

    // Main scheduling loop
    while (completed_processes < n) {
        // Check if I/O is complete for any process
        if (current_io_process != -1 && io_busy_until <= current_time) {
            processes[current_io_process].in_io = 0;
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

        // Find the process with shortest remaining time that is ready
        int next_process = find_shortest_remaining_time(processes, n, current_time);
        
        // PREEMPTIVE: Determine if we need to preempt the current process
        if (next_process != -1 && current_process != -1 && next_process != current_process) {
            if (processes[next_process].remaining_cpu_burst < processes[current_process].remaining_cpu_burst) {
                // Update last execution time for current process before preemption
                processes[current_process].last_execution_time = current_time;
                // Preempt current process
                current_process = next_process;
                
                // First time this process runs, record response time
                if (!processes[current_process].started) {
                    processes[current_process].response_time = current_time - processes[current_process].arrival_time;
                    processes[current_process].started = 1;
                }
            }
        } else if (current_process == -1) {
            // If CPU is idle, assign the next process
            current_process = next_process;
            
            // First time this process runs, record response time
            if (current_process != -1 && !processes[current_process].started) {
                processes[current_process].response_time = current_time - processes[current_process].arrival_time;
                processes[current_process].started = 1;
                processes[current_process].last_execution_time = current_time;
            }
        }

        // If there's no process to run and no I/O happening, just advance time
        if (current_process == -1 && completed_processes < n) {
            // Add to timeline
            if (timeline_len < 999) {
                timeline[timeline_len++] = '-';
            }
            current_time++;
            continue;
        }

        // Execute current process
        if (current_process != -1) {
            // Add to timeline
            if (timeline_len < 999) {
                timeline[timeline_len++] = processes[current_process].pid[1]; // Using second character of PID for timeline
            }
            
            // Update waiting time if process was previously executed and preempted
            if (processes[current_process].last_execution_time != current_time && 
                processes[current_process].last_execution_time != -1) {
                processes[current_process].waiting_time += (current_time - processes[current_process].last_execution_time);
            }
            processes[current_process].last_execution_time = current_time + 1; // Update for next potential preemption
            
            processes[current_process].current_cpu_count++;
            processes[current_process].remaining_cpu_burst--;
            
            // Check if process needs I/O after certain CPU units
            if (processes[current_process].current_cpu_count % processes[current_process].cpu_units_per_io == 0 && 
                processes[current_process].remaining_cpu_burst > 0) {
                
                processes[current_process].in_io = 1;
                io_queue[io_queue_count++] = current_process;
                current_process = -1;
            }
            
            // Check if process is complete
            else if (processes[current_process].remaining_cpu_burst == 0) {
                processes[current_process].completion_time = current_time + 1;
                processes[current_process].turnaround_time = processes[current_process].completion_time - processes[current_process].arrival_time;
                
                // For first process or process that never preempted, calculate waiting time traditionally
                if (processes[current_process].waiting_time == 0) {
                    processes[current_process].waiting_time = processes[current_process].turnaround_time - processes[current_process].total_cpu_burst;
                } else {
                    // For processes that were preempted, we already counted waiting time during execution
                    // We just need to adjust if there was initial waiting
                    if (processes[current_process].response_time > 0) {
                        processes[current_process].waiting_time += processes[current_process].response_time;
                    }
                }
                
                completed_processes++;
                current_process = -1;
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
    srtf_scheduler(filename);
    return 0;
}