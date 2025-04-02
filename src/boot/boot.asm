[bits 16]          ; Real Mode (16-bit)
[org 0x7c00]      ; Boot sector origin

print_string:
    mov ah, 0x0e   ; BIOS teletype function
.loop:
    lodsb          ; Load byte from SI into AL
    cmp al, 0      ; Check for null terminator
    je .done       ; If null, stop
    int 0x10       ; Print character
    jmp .loop      ; Next character
.done:
    ret

start:
    mov si, boot_msg
    call print_string

boot_msg db 'Quantum OS Booting...', 0

cli                    ; Disable interrupts
lgdt [gdt_descriptor]  ; Load GDT

gdt_start:
    dq 0x0             ; Null descriptor
gdt_code:
    dw 0xffff         ; Limit
    dw 0x0            ; Base (low)
    db 0x0            ; Base (mid)
    db 10011010b      ; Access byte (code segment)
    db 11001111b      ; Flags + limit
    db 0x0            ; Base (high)
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b      ; Access byte (data segment)
    db 11001111b
    db 0x0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size
    dd gdt_start                ; Address