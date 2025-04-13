[bits 32]

global load_idt
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15

extern isr_handler

; IDT Descriptor
idt_descriptor:
    dw idt_end - idt_start - 1
    dd idt_start

; Load IDT
load_idt:
    lidt [idt_descriptor]
    ret

; Default ISR stub
%macro ISR_NOERRCODE 1
isr%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
isr%1:
    cli
    push byte %1
    jmp isr_common_stub
%endmacro

; Create ISR stubs
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRCODE 15

; Common ISR stub
isr_common_stub:
    pusha
    
    mov ax, ds
    push eax
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    call isr_handler
    
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa
    add esp, 8
    sti
    iret

; IDT
idt_start:
    times 256 dd 0, 0
idt_end:
