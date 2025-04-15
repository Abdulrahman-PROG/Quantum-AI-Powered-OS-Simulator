[GLOBAL load_idt_asm]
[GLOBAL isr_common_stub]
[GLOBAL irq_common_stub]
[EXTERN isr_handler]
[EXTERN irq_handler]

; Load IDT
load_idt_asm:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]  ; Get pointer to IDTR
    lidt [eax]          ; Load IDT
    pop ebp
    ret

; Common ISR stub
isr_common_stub:
    pusha               ; Push all registers
    mov ax, ds
    push eax           ; Save data segment

    mov ax, 0x10       ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler   ; Call C handler

    pop eax            ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa               ; Restore registers
    add esp, 8         ; Clean up stack
    iret               ; Return from interrupt

; Common IRQ stub
irq_common_stub:
    pusha              ; Push all registers
    mov ax, ds
    push eax          ; Save data segment

    mov ax, 0x10      ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_handler  ; Call C handler

    pop ebx           ; Restore data segment
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa              ; Restore registers
    add esp, 8        ; Clean up stack
    iret              ; Return from interrupt 