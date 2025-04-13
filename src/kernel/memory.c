#include "../include/types.h"
#include "../include/memory.h"

#define PAGE_SIZE 4096
#define MAX_FRAMES 1024

static uint32_t frames[MAX_FRAMES / 32];  // Bitmap for frame allocation
static uint32_t total_frames = 0;

void init_memory(uint32_t mem_size) {
    total_frames = mem_size / PAGE_SIZE;
    // Initialize all frames as free (0)
    for (uint32_t i = 0; i < MAX_FRAMES / 32; i++) {
        frames[i] = 0;
    }
}

void* alloc_frame() {
    for (uint32_t i = 0; i < MAX_FRAMES / 32; i++) {
        if (frames[i] != 0xFFFFFFFF) {  // If not all frames are allocated
            for (uint32_t j = 0; j < 32; j++) {
                if (!(frames[i] & (1 << j))) {
                    frames[i] |= (1 << j);  // Mark frame as allocated
                    return (void*)((i * 32 + j) * PAGE_SIZE);
                }
            }
        }
    }
    return NULL;  // No free frames
}

void free_frame(void* addr) {
    uint32_t frame = (uint32_t)addr / PAGE_SIZE;
    uint32_t index = frame / 32;
    uint32_t offset = frame % 32;
    
    if (index < MAX_FRAMES / 32) {
        frames[index] &= ~(1 << offset);  // Mark frame as free
    }
}

uint32_t get_total_frames() {
    return total_frames;
}

uint32_t get_free_frames() {
    uint32_t free = 0;
    for (uint32_t i = 0; i < MAX_FRAMES / 32; i++) {
        for (uint32_t j = 0; j < 32; j++) {
            if (!(frames[i] & (1 << j))) {
                free++;
            }
        }
    }
    return free;
}
