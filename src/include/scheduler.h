#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"
#include "process.h"

void init_scheduler();
int create_process(void (*entry_point)(), uint32_t stack_size);
void schedule();
Process* get_current_process();
int get_process_count();

#endif // SCHEDULER_H 