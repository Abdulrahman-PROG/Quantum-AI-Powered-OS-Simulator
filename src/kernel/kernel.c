#include "../include/stdint.h"
#include "../include/string.h"
#include "../include/types.h"
#include "../include/memory.h"
#include "../include/process.h"
#include "../include/scheduler.h"
#include "../include/vga.h"
#include "../include/interrupts.h"

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

// Box drawing characters (single bytes)
#define BOX_TOP_LEFT     '+'
#define BOX_TOP_RIGHT    '+'
#define BOX_BOTTOM_LEFT  '+'
#define BOX_BOTTOM_RIGHT '+'
#define BOX_HORIZONTAL   '-'
#define BOX_VERTICAL     '|'

// Current cursor position and color
static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t current_color = (VGA_BLACK << 4) | VGA_LIGHT_GREY;

// Animation frames for loading
static const char* loading_frames[] = {"|", "/", "-", "\\"};
static int current_frame = 0;

// Delay function
void delay(int count) {
    for (volatile int i = 0; i < count; i++) {
        for (volatile int j = 0; j < 1000; j++) {}
    }
}

// Set cursor position
void set_cursor_pos(int x, int y) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        cursor_x = x;
        cursor_y = y;
        update_cursor();
    }
}

// Save cursor position
void save_cursor_pos(int* x, int* y) {
    *x = cursor_x;
    *y = cursor_y;
}

// Draw a box
void draw_box(int x, int y, int width, int height, uint8_t color) {
    uint8_t old_color = current_color;
    current_color = color;
    
    // Draw top border
    set_cursor_pos(x, y);
    putchar(BOX_TOP_LEFT);
    for (int i = 1; i < width - 1; i++) putchar(BOX_HORIZONTAL);
    putchar(BOX_TOP_RIGHT);
    
    // Draw sides
    for (int i = 1; i < height - 1; i++) {
        set_cursor_pos(x, y + i);
        putchar(BOX_VERTICAL);
        set_cursor_pos(x + width - 1, y + i);
        putchar(BOX_VERTICAL);
    }
    
    // Draw bottom border
    set_cursor_pos(x, y + height - 1);
    putchar(BOX_BOTTOM_LEFT);
    for (int i = 1; i < width - 1; i++) putchar(BOX_HORIZONTAL);
    putchar(BOX_BOTTOM_RIGHT);
    
    current_color = old_color;
}

// Show loading animation
void show_loading(const char* message, int x, int y, int duration) {
    int old_x, old_y;
    save_cursor_pos(&old_x, &old_y);
    
    for (int i = 0; i < duration; i++) {
        set_cursor_pos(x, y);
        print(message);
        print(" ");
        print_colored(loading_frames[current_frame], VGA_LIGHT_CYAN, VGA_BLACK);
        current_frame = (current_frame + 1) % 4;
        delay(100);
    }
    
    set_cursor_pos(x, y);
    for (int i = 0; i < strlen(message) + 2; i++) {
        putchar(' ');
    }
    
    set_cursor_pos(old_x, old_y);
}

// Draw progress bar
void draw_progress_bar(int x, int y, int width, int progress) {
    int filled = (progress * (width - 2)) / 100;
    
    set_cursor_pos(x, y);
    putchar('[');
    
    for (int i = 0; i < width - 2; i++) {
        if (i < filled) {
            print_colored("█", VGA_LIGHT_GREEN, VGA_BLACK);
        } else {
            print_colored("░", VGA_DARK_GREY, VGA_BLACK);
        }
    }
    
    putchar(']');
    
    // Print percentage
    char percent[5];
    int len = 0;
    int temp = progress;
    do {
        percent[len++] = '0' + (temp % 10);
        temp /= 10;
    } while (temp > 0);
    
    print(" ");
    while (len > 0) {
        putchar(percent[--len]);
    }
    print("%");
}

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

void test_process() {
    static int counter = 0;
    while (1) {
        print_colored("Process: ", VGA_LIGHT_CYAN, VGA_BLACK);
        print_int(counter++);
        print("\n");
        for (volatile int i = 0; i < 1000000; i++);  // Simple delay
    }
}

void init_vga() {
    // Set VGA mode 3 (80x25 text mode)
    __asm__ volatile (
        "mov $0x03, %al\n"
        "mov $0x00, %ah\n"
        "int $0x10\n"
    );
    
    // Enable hardware cursor
    __asm__ volatile (
        "mov $0x01, %ah\n"
        "mov $0x07, %ch\n"
        "mov $0x0F, %cl\n"
        "int $0x10\n"
    );
}

void _start(void) __attribute__((section(".text.start")));
void _start(void) {
    // Initialize VGA mode first
    init_vga();
    
    // Set initial color scheme and clear screen
    set_color(VGA_LIGHT_GREY, VGA_BLACK);
    clear_screen();
    
    // Draw main box
    draw_box(0, 0, VGA_WIDTH, VGA_HEIGHT, (VGA_BLACK << 4) | VGA_LIGHT_CYAN);
    
    // Print fancy header
    set_cursor_pos(2, 2);
    print_colored("╔════════════════════════════════════════════════════════╗\n", VGA_LIGHT_CYAN, VGA_BLACK);
    set_cursor_pos(2, 3);
    print_colored("║              Quantum OS Kernel Loading                 ║\n", VGA_LIGHT_MAGENTA, VGA_BLACK);
    set_cursor_pos(2, 4);
    print_colored("╚════════════════════════════════════════════════════════╝\n", VGA_LIGHT_CYAN, VGA_BLACK);
    
    // Initialize components with animations
    set_cursor_pos(4, 6);
    show_loading("Initializing memory", 4, 6, 10);
    init_memory(16 * 1024 * 1024);
    print_status("Memory initialized\n", 1);
    
    set_cursor_pos(4, 7);
    show_loading("Setting up interrupts", 4, 7, 8);
    load_idt();
    print_status("Interrupts initialized\n", 1);
    
    set_cursor_pos(4, 8);
    show_loading("Starting scheduler", 4, 8, 6);
    init_scheduler();
    print_status("Scheduler initialized\n", 1);
    
    // Show overall progress
    set_cursor_pos(4, 10);
    print_colored("System Initialization Progress:\n", VGA_WHITE, VGA_BLACK);
    for (int i = 0; i <= 100; i += 10) {
        set_cursor_pos(4, 11);
        draw_progress_bar(4, 11, 50, i);
        delay(50);
    }
    
    // Create a test process
    set_cursor_pos(4, 13);
    int pid = create_process(test_process, 4096);
    if (pid != -1) {
        print_colored("Process created with PID: ", VGA_LIGHT_GREEN, VGA_BLACK);
        print_int(pid);
        print("\n");
    } else {
        print_status("Failed to create process\n", 0);
    }
    
    // Draw completion box
    draw_box(2, 15, 76, 7, (VGA_BLACK << 4) | VGA_LIGHT_GREEN);
    set_cursor_pos(4, 17);
    print_colored("    ✓ Kernel initialization complete    \n", VGA_LIGHT_GREEN, VGA_BLACK);
    set_cursor_pos(4, 18);
    print_colored("    System is ready    \n", VGA_LIGHT_GREEN, VGA_BLACK);
    
    // Main kernel loop
    while (1) {
        schedule();
        delay(100);
    }
}