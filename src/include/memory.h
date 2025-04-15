#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

void init_memory(size_t size);
void* kmalloc(size_t size);
void kfree(void* ptr);
size_t get_used_memory(void);
size_t get_total_memory(void);

#endif // MEMORY_H 