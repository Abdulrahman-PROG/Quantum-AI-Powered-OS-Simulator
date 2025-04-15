#include "../include/types.h"
#include "../include/process.h"
#include "../include/scheduler.h"
#include "../include/memory.h"

#define MAX_PROCESSES 32
#define STACK_SIZE 4096

static Process processes[MAX_PROCESSES];
static int current_process = -1;
static int num_processes = 0;

void init_scheduler(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].state = PROCESS_UNUSED;
    }
    current_process = -1;
    num_processes = 0;
}

int create_process(void (*entry_point)(void), size_t stack_size) {
    if (num_processes >= MAX_PROCESSES) {
        return -1;
    }

    // Find free process slot
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
    processes[pid].stack_pointer = (uint32_t)kmalloc(stack_size);

    if (!processes[pid].stack_pointer) {
        return -1;
    }

    num_processes++;
    if (current_process == -1) {
        current_process = pid;
        processes[pid].state = PROCESS_RUNNING;
    }

    return pid;
}

void schedule(void) {
    if (num_processes == 0) {
        return;
    }

    // Simple round-robin scheduling
    int next_process = current_process;
    do {
        next_process = (next_process + 1) % MAX_PROCESSES;
    } while (processes[next_process].state != PROCESS_READY);

    if (current_process != -1) {
        processes[current_process].state = PROCESS_READY;
    }

    current_process = next_process;
    processes[current_process].state = PROCESS_RUNNING;
}

Process* get_current_process(void) {
    if (current_process == -1) {
        return NULL;
    }
    return &processes[current_process];
}

int get_process_count(void) {
    return num_processes;
}
