#include "../include/interrupts.h"
#include "../include/vga.h"

// IDT array
static struct idt_entry idt[256];
static struct idtr idtr;

// Assembly functions declared in assembly file
extern void load_idt_asm(struct idtr* idtr);
extern void isr_common_stub(void);
extern void irq_common_stub(void);

// Generic interrupt handlers
void isr_handler(void) {
    print_colored("Interrupt Service Routine called\n", 0x07, 0x00);
}

void irq_handler(void) {
    print_colored("Hardware Interrupt Request received\n", 0x07, 0x00);
}

// Initialize the IDT
void init_idt(void) {
    // Set up IDT pointer
    idtr.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtr.base = (uint32_t)&idt;

    // Clear IDT
    for (int i = 0; i < 256; i++) {
        set_idt_gate(i, 0, 0x08, 0x8E);  // Kernel code segment and interrupt gate
    }

    // Set up exception handlers
    set_idt_gate(0, (uint32_t)isr0_handler, 0x08, 0x8E);   // Divide by zero
    set_idt_gate(13, (uint32_t)isr13_handler, 0x08, 0x8E); // GPF
    set_idt_gate(14, (uint32_t)isr14_handler, 0x08, 0x8E); // Page fault

    // Set up IRQ handlers
    set_idt_gate(32, (uint32_t)irq0_handler, 0x08, 0x8E);  // Timer
    set_idt_gate(33, (uint32_t)irq1_handler, 0x08, 0x8E);  // Keyboard
    set_idt_gate(46, (uint32_t)irq14_handler, 0x08, 0x8E); // Primary ATA
    set_idt_gate(47, (uint32_t)irq15_handler, 0x08, 0x8E); // Secondary ATA

    // Load IDT
    load_idt();
}

// Set an IDT gate
void set_idt_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

// Load the IDT
void load_idt(void) {
    load_idt_asm(&idtr);
}

// Default handlers
void irq0_handler(void) {
    // Timer interrupt handler
    print_colored("Timer tick\n", 0x07, 0x00);
}

void irq1_handler(void) {
    // Keyboard interrupt handler
    print_colored("Keyboard input\n", 0x07, 0x00);
}

void irq14_handler(void) {
    // Primary ATA handler
    print_colored("Primary ATA interrupt\n", 0x07, 0x00);
}

void irq15_handler(void) {
    // Secondary ATA handler
    print_colored("Secondary ATA interrupt\n", 0x07, 0x00);
}

void isr0_handler(void) {
    // Division by zero handler
    print_colored("Division by zero error!\n", 0x04, 0x00);
    while(1); // Halt the system
}

void isr13_handler(void) {
    // General Protection Fault handler
    print_colored("General Protection Fault!\n", 0x04, 0x00);
    while(1); // Halt the system
}

void isr14_handler(void) {
    // Page Fault handler
    print_colored("Page Fault!\n", 0x04, 0x00);
    while(1); // Halt the system
}

// Helper functions
void enable_interrupts(void) {
    __asm__ volatile("sti");
}

void disable_interrupts(void) {
    __asm__ volatile("cli");
} 