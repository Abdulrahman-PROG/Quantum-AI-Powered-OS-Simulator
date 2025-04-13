#include "../include/types.h"
#include "../include/process.h"
#include "../include/scheduler.h"

#define MAX_PROCESSES 16

static Process processes[MAX_PROCESSES];
static int current_process = -1;
static int process_count = 0;

void init_scheduler() {
    process_count = 0;
    current_process = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].state = PROCESS_UNUSED;
    }
}

int create_process(void (*entry_point)(), uint32_t stack_size) {
    if (process_count >= MAX_PROCESSES) {
        return -1;  // No more process slots available
    }

    // Find an unused process slot
    int pid = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_UNUSED) {
            pid = i;
            break;
        }
    }

    if (pid == -1) {
        return -1;
    }

    // Initialize process
    processes[pid].pid = pid;
    processes[pid].state = PROCESS_READY;
    processes[pid].entry_point = entry_point;
    processes[pid].stack_size = stack_size;
    processes[pid].stack_pointer = 0;  // Will be set when process is first scheduled

    process_count++;
    return pid;
}

void schedule() {
    if (process_count == 0) {
        return;  // No processes to schedule
    }

    // Find next ready process
    int next_process = current_process;
    do {
        next_process = (next_process + 1) % MAX_PROCESSES;
    } while (processes[next_process].state != PROCESS_READY && 
             processes[next_process].state != PROCESS_RUNNING);

    if (next_process == current_process) {
        return;  // No other process to switch to
    }

    // Save current process state if it exists
    if (current_process != -1 && processes[current_process].state == PROCESS_RUNNING) {
        processes[current_process].state = PROCESS_READY;
    }

    // Switch to next process
    current_process = next_process;
    processes[current_process].state = PROCESS_RUNNING;
}

Process* get_current_process() {
    if (current_process == -1) {
        return NULL;
    }
    return &processes[current_process];
}

int get_process_count() {
    return process_count;
}
