#include "../include/types.h"
#include "../include/memory.h"
#include "../include/process.h"
#include "../include/scheduler.h"

extern void load_idt();

void print(const char* str) {
    char* vga = (char*)0xb8000;  // VGA memory address
    static int pos = 0;
    while (*str) {
        vga[pos++] = *str++;     // Character
        vga[pos++] = 0x07;       // White on black
    }
}

void isr_handler() {
    print("Interrupt occurred!\n");
}

void test_process() {
    while (1) {
        print("Test process running...\n");
        for (volatile int i = 0; i < 1000000; i++);  // Simple delay
    }
}

void kernel_main(void) __attribute__((noreturn));
void kernel_main(void) {
    print("Quantum OS Kernel Loading...\n");
    
    // Initialize memory management
    init_memory(16 * 1024 * 1024);  // 16MB of memory
    print("Memory initialized\n");
    
    // Load IDT
    load_idt();
    print("Interrupts initialized\n");
    
    // Initialize scheduler
    init_scheduler();
    print("Scheduler initialized\n");
    
    // Create a test process
    int pid = create_process(test_process, 4096);
    if (pid != -1) {
        print("Test process created\n");
    } else {
        print("Failed to create test process\n");
    }
    
    print("Kernel initialization complete\n");
    
    // Main kernel loop
    while (1) {
        schedule();
        for (volatile int i = 0; i < 1000000; i++);  // Simple delay
    }
}