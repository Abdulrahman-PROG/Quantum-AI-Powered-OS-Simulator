[bits 16]          ; Real Mode (16-bit)
[org 0x7c00]

global _start
_start:
    ; Set up segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Print boot message
    mov si, boot_msg
    call print_string

    ; Load kernel from disk
    mov ah, 0x02    ; BIOS read sector function
    mov al, 32      ; Number of sectors to read
    mov ch, 0       ; Cylinder number
    mov cl, 2       ; Sector number (1 is boot sector)
    mov dh, 0       ; Head number
    mov dl, 0x80    ; Drive number (first hard disk)
    mov bx, 0x1000  ; Buffer address
    int 0x13
    jc disk_error

    ; Switch to protected mode
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode

disk_error:
    mov si, disk_error_msg
    call print_string
    jmp $

print_string:
    mov ah, 0x0e   ; BIOS teletype function
.loop:
    lodsb          ; Load byte from SI into AL
    test al, al     ; Check for null terminator
    jz .done        ; If null, stop
    int 0x10        ; Print character
    jmp .loop       ; Next character
.done:
    ret

boot_msg db 'Booting Quantum OS...', 13, 10, 0
disk_error_msg db 'Error loading kernel!', 13, 10, 0

[bits 32]
protected_mode:
    mov ax, 0x10       ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000   ; Set stack pointer
    jmp 0x1000         ; Jump to kernel

gdt_start:
    dq 0x0000000000000000 ; Null descriptor
gdt_code:
    dw 0xffff    ; Limit
    dw 0x0000    ; Base (low)
    db 0x00      ; Base (middle)
    db 10011010b ; Access
    db 11001111b ; Granularity
    db 0x00      ; Base (high)
gdt_data:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510-($-$$) db 0  ; Pad with zeros
dw 0xaa55              ; Boot signature

