#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

typedef enum {
    PROCESS_UNUSED,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} ProcessState;

typedef struct {
    int pid;
    ProcessState state;
    void (*entry_point)();
    uint32_t stack_size;
    uint32_t stack_pointer;
} Process;

#endif // PROCESS_H
