#include "../include/process.h"

Process processes[MAX_PROCESSES];
int current_process = -1;
int process_count = 0;

void print(const char* str) {
    char* vga = (char*)0xb8000;  // VGA memory address
    static int pos = 0;
    while (*str) {
        vga[pos++] = *str++;     // Character
        vga[pos++] = 0x07;       // White on black
    }
}


void kernel_main(void) __attribute__((noreturn));
void kernel_main(void) {
    print("Quantum OS Kernel Loaded!\n");
    while (1); // Infinite loop to keep the kernel running
}