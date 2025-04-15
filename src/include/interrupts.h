#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

// Interrupt Descriptor Table entry structure
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

// IDTR structure
struct idtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Function declarations
void init_idt(void);
void set_idt_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);
void load_idt(void);

// Common IRQ handlers
void irq0_handler(void);  // Timer
void irq1_handler(void);  // Keyboard
void irq14_handler(void); // Primary ATA
void irq15_handler(void); // Secondary ATA

// Exception handlers
void isr0_handler(void);  // Division by zero
void isr13_handler(void); // General Protection Fault
void isr14_handler(void); // Page Fault

// Helper functions
void enable_interrupts(void);
void disable_interrupts(void);

#endif // INTERRUPTS_H 