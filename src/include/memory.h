#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

void init_memory(uint32_t mem_size);
void* alloc_frame();
void free_frame(void* addr);
uint32_t get_total_frames();
uint32_t get_free_frames();

#endif // MEMORY_H 