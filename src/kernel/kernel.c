#include "../include/types.h"
#include "../include/memory.h"
#include "../include/process.h"
#include "../include/scheduler.h"

extern void load_idt();

// VGA buffer constants
#define VGA_BUFFER 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// VGA colors
#define VGA_BLACK         0x0
#define VGA_BLUE          0x1
#define VGA_GREEN         0x2
#define VGA_CYAN          0x3
#define VGA_RED           0x4
#define VGA_MAGENTA       0x5
#define VGA_BROWN         0x6
#define VGA_LIGHT_GREY    0x7
#define VGA_DARK_GREY     0x8
#define VGA_LIGHT_BLUE    0x9
#define VGA_LIGHT_GREEN   0xA
#define VGA_LIGHT_CYAN    0xB
#define VGA_LIGHT_RED     0xC
#define VGA_LIGHT_MAGENTA 0xD
#define VGA_LIGHT_BROWN   0xE
#define VGA_WHITE         0xF

// Current cursor position and color
static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t current_color = (VGA_LIGHT_GREY << 4) | VGA_BLACK; // Background | Foreground

// Set the color for text output
void set_color(uint8_t foreground, uint8_t background) {
    current_color = (background << 4) | foreground;
}

// Clear the screen with current color
void clear_screen() {
    uint16_t* vga = (uint16_t*)VGA_BUFFER;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = (current_color << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
}

// Update hardware cursor
void update_cursor() {
    uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
    uint8_t high = (pos >> 8) & 0xFF;
    uint8_t low = pos & 0xFF;
    
    // Tell the VGA board we are setting the high cursor byte
    __asm__ volatile ("outb %%al, %%dx" : : "a"(0x0E), "d"(0x3D4));
    __asm__ volatile ("outb %%al, %%dx" : : "a"(high), "d"(0x3D5));
    
    // Tell the VGA board we are setting the low cursor byte
    __asm__ volatile ("outb %%al, %%dx" : : "a"(0x0F), "d"(0x3D4));
    __asm__ volatile ("outb %%al, %%dx" : : "a"(low), "d"(0x3D5));
}

// Scroll the screen if needed
void scroll() {
    if (cursor_y >= VGA_HEIGHT) {
        uint16_t* vga = (uint16_t*)VGA_BUFFER;
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            vga[i] = vga[i + VGA_WIDTH];
        }
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            vga[i] = (current_color << 8) | ' ';
        }
        cursor_y--;
    }
}

// Print a character with current color
void putchar(char c) {
    uint16_t* vga = (uint16_t*)VGA_BUFFER;
    
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        scroll();
        update_cursor();
        return;
    }
    
    vga[cursor_y * VGA_WIDTH + cursor_x] = (current_color << 8) | c;
    cursor_x++;
    
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        scroll();
    }
    
    update_cursor();
}

// Print a string with current color
void print(const char* str) {
    while (*str) {
        putchar(*str++);
    }
}

// Print a string with specific colors
void print_colored(const char* str, uint8_t foreground, uint8_t background) {
    uint8_t old_color = current_color;
    set_color(foreground, background);
    print(str);
    current_color = old_color;
}

// Print an integer
void print_int(int num) {
    if (num == 0) {
        putchar('0');
        return;
    }
    
    if (num < 0) {
        putchar('-');
        num = -num;
    }
    
    char buffer[32];
    int i = 0;
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        putchar(buffer[--i]);
    }
}

// Print a status message with color
void print_status(const char* msg, int success) {
    if (success) {
        print_colored("[", VGA_WHITE, VGA_BLACK);
        print_colored("OK", VGA_LIGHT_GREEN, VGA_BLACK);
        print_colored("] ", VGA_WHITE, VGA_BLACK);
        print(msg);
    } else {
        print_colored("[", VGA_WHITE, VGA_BLACK);
        print_colored("ERROR", VGA_LIGHT_RED, VGA_BLACK);
        print_colored("] ", VGA_WHITE, VGA_BLACK);
        print(msg);
    }
}

void isr_handler() {
    print_status("Interrupt occurred!\n", 1);
}

void test_process() {
    static int counter = 0;
    while (1) {
        print_colored("Process: ", VGA_LIGHT_CYAN, VGA_BLACK);
        print_int(counter++);
        print("\n");
        for (volatile int i = 0; i < 1000000; i++);  // Simple delay
    }
}

void _start(void) __attribute__((section(".text.start")));
void _start(void) {
    // Set initial color scheme
    set_color(VGA_WHITE, VGA_BLACK);
    clear_screen();
    
    // Print fancy header
    print_colored("=====================================\n", VGA_LIGHT_CYAN, VGA_BLACK);
    print_colored("      Quantum OS Kernel Loading     \n", VGA_LIGHT_MAGENTA, VGA_BLACK);
    print_colored("=====================================\n\n", VGA_LIGHT_CYAN, VGA_BLACK);
    
    // Initialize memory management
    init_memory(16 * 1024 * 1024);  // 16MB of memory
    print_status("Memory initialized\n", 1);
    
    // Load IDT
    load_idt();
    print_status("Interrupts initialized\n", 1);
    
    // Initialize scheduler
    init_scheduler();
    print_status("Scheduler initialized\n", 1);
    
    // Create a test process
    int pid = create_process(test_process, 4096);
    if (pid != -1) {
        print_colored("Process created with PID: ", VGA_LIGHT_GREEN, VGA_BLACK);
        print_int(pid);
        print("\n");
    } else {
        print_status("Failed to create process\n", 0);
    }
    
    print("\n");
    print_colored("=====================================\n", VGA_LIGHT_CYAN, VGA_BLACK);
    print_colored("    Kernel initialization complete   \n", VGA_LIGHT_GREEN, VGA_BLACK);
    print_colored("=====================================\n", VGA_LIGHT_CYAN, VGA_BLACK);
    
    // Main kernel loop
    while (1) {
        schedule();
        for (volatile int i = 0; i < 1000000; i++);  // Simple delay
    }
}