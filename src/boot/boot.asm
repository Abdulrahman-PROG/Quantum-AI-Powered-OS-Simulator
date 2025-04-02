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