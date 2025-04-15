#include "../include/types.h"
#include "../include/memory.h"
#include "../include/string.h"

#define PAGE_SIZE 4096
#define MAX_FRAMES 1024

static uint32_t frames[MAX_FRAMES / 32];  // Bitmap for frame allocation
static uint32_t total_frames = 0;

// Simple memory allocator implementation
static uint8_t* memory_start = NULL;
static size_t memory_size = 0;
static size_t used_memory = 0;

void init_memory(size_t size) {
    memory_start = (uint8_t*)0x100000; // Start at 1MB
    memory_size = size;
    used_memory = 0;
    memset(memory_start, 0, memory_size);

    total_frames = size / PAGE_SIZE;
    // Initialize all frames as free (0)
    for (uint32_t i = 0; i < MAX_FRAMES / 32; i++) {
        frames[i] = 0;
    }
}

void* kmalloc(size_t size) {
    if (used_memory + size > memory_size) {
        return NULL;
    }

    void* ptr = memory_start + used_memory;
    used_memory += size;
    return ptr;
}

void kfree(void* ptr) {
    // Simple implementation - no actual freeing
    (void)ptr;
}

size_t get_used_memory(void) {
    return used_memory;
}

size_t get_total_memory(void) {
    return memory_size;
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
