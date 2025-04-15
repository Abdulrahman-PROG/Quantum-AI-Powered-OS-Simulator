#include "../include/types.h"
#include "../include/memory.h"
#include "../include/process.h"
#include "../include/scheduler.h"

extern void load_idt();

// VGA buffer constants
#define VGA_BUFFER 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR_WHITE_ON_BLACK 0x0F

// Current cursor position
static int cursor_x = 0;
static int cursor_y = 0;

// Clear the screen
void clear_screen() {
    uint16_t* vga = (uint16_t*)VGA_BUFFER;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = (VGA_COLOR_WHITE_ON_BLACK << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
}

// Scroll the screen if needed
void scroll() {
    if (cursor_y >= VGA_HEIGHT) {
        uint16_t* vga = (uint16_t*)VGA_BUFFER;
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            vga[i] = vga[i + VGA_WIDTH];
        }
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            vga[i] = (VGA_COLOR_WHITE_ON_BLACK << 8) | ' ';
        }
        cursor_y--;
    }
}

// Print a character
void putchar(char c) {
    uint16_t* vga = (uint16_t*)VGA_BUFFER;
    
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        scroll();
        return;
    }
    
    vga[cursor_y * VGA_WIDTH + cursor_x] = (VGA_COLOR_WHITE_ON_BLACK << 8) | c;
    cursor_x++;
    
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        scroll();
    }
}

// Print a string
void print(const char* str) {
    while (*str) {
        putchar(*str++);
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

void _start(void) __attribute__((section(".text.start")));
void _start(void) {
    clear_screen();
    print("=================================\n");
    print("    Quantum OS Kernel Loading    \n");
    print("=================================\n\n");
    
    // Initialize memory management
    init_memory(16 * 1024 * 1024);  // 16MB of memory
    print("[OK] Memory initialized\n");
    
    // Load IDT
    load_idt();
    print("[OK] Interrupts initialized\n");
    
    // Initialize scheduler
    init_scheduler();
    print("[OK] Scheduler initialized\n");
    
    // Create a test process
    int pid = create_process(test_process, 4096);
    if (pid != -1) {
        print("[OK] Test process created with PID: ");
        // Convert PID to string and print it
        char pid_str[10];
        int i = 0;
        int temp_pid = pid;
        do {
            pid_str[i++] = '0' + (temp_pid % 10);
            temp_pid /= 10;
        } while (temp_pid > 0);
        while (i > 0) {
            putchar(pid_str[--i]);
        }
        print("\n");
    } else {
        print("[ERROR] Failed to create test process\n");
    }
    
    print("\n=================================\n");
    print("Kernel initialization complete\n");
    print("=================================\n");
    
    // Main kernel loop
    while (1) {
        schedule();
        for (volatile int i = 0; i < 1000000; i++);  // Simple delay
    }
}