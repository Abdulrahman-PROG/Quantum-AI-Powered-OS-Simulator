#ifndef VGA_H
#define VGA_H

#include "stdint.h"

// VGA buffer manipulation functions
void set_cursor_pos(int x, int y);
void save_cursor_pos(int* x, int* y);
void update_cursor(void);
void clear_screen(void);
void set_color(uint8_t foreground, uint8_t background);
void putchar(char c);
void print(const char* str);
void print_colored(const char* str, uint8_t foreground, uint8_t background);

// UI drawing functions
void draw_box(int x, int y, int width, int height, uint8_t color);
void draw_progress_bar(int x, int y, int width, int progress);
void show_loading(const char* message, int x, int y, int duration);

// Utility functions
void delay(int count);
void scroll(void);

#endif // VGA_H 