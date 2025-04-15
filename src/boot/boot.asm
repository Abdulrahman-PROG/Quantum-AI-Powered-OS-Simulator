[bits 16]          ; Real Mode (16-bit)
[org 0x7c00]

KERNEL_OFFSET equ 0x1000    ; Kernel will be loaded at 0x1000
KERNEL_SIZE equ 32         ; Number of sectors to read (16KB)

start:
    ; Initialize segment registers
    cli                     ; Clear interrupts
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, 0x7c00
    sti                     ; Enable interrupts

    ; Save boot drive number
    mov [BOOT_DRIVE], dl

    ; Print welcome message
    mov si, MSG_REAL_MODE
    call print_string

    ; Load kernel
    call load_kernel

    ; Switch to protected mode
    call switch_to_pm

    jmp $                   ; Should never get here

load_kernel:
    mov si, MSG_LOAD_KERNEL
    call print_string

    mov ah, 0x02           ; BIOS read sector function
    mov al, KERNEL_SIZE    ; Number of sectors to read
    mov ch, 0              ; Cylinder 0
    mov dh, 0              ; Head 0
    mov cl, 2              ; Start from sector 2
    mov dl, [BOOT_DRIVE]   ; Drive number
    mov bx, KERNEL_OFFSET  ; Buffer to load to
    int 0x13              ; BIOS interrupt

    jc disk_error         ; If error (carry flag set)
    
    cmp al, KERNEL_SIZE   ; Check if we read all sectors
    jne disk_error        ; If not all sectors read

    ret

disk_error:
    mov si, MSG_DISK_ERROR
    call print_string
    jmp $

print_string:
    pusha
    mov ah, 0x0e           ; BIOS teletype function
.loop:
    lodsb                  ; Load next character
    test al, al           ; Check for null terminator
    jz .done              ; If null, we're done
    int 0x10              ; Print character
    jmp .loop
.done:
    popa
    ret

switch_to_pm:
    cli                    ; Turn off interrupts
    lgdt [gdt_descriptor]  ; Load GDT descriptor
    
    mov eax, cr0          ; Get current cr0
    or eax, 0x1           ; Set PE bit
    mov cr0, eax          ; Switch to protected mode
    
    jmp CODE_SEG:init_pm  ; Far jump to flush CPU pipeline

[bits 32]
init_pm:
    mov ax, DATA_SEG      ; Update segment registers
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000      ; Set up stack
    mov esp, ebp

    call KERNEL_OFFSET    ; Jump to kernel

; GDT
gdt_start:
    dq 0x0                ; Null descriptor

gdt_code:                 ; Code segment descriptor
    dw 0xFFFF            ; Limit (bits 0-15)
    dw 0x0               ; Base (bits 0-15)
    db 0x0               ; Base (bits 16-23)
    db 10011010b         ; Access byte
    db 11001111b         ; Flags + Limit (bits 16-19)
    db 0x0               ; Base (bits 24-31)

gdt_data:                 ; Data segment descriptor
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size (16 bits)
    dd gdt_start                ; Address (32 bits)

; Constants
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Data
BOOT_DRIVE db 0
MSG_REAL_MODE db "Starting Quantum OS...", 13, 10, 0
MSG_LOAD_KERNEL db "Loading kernel...", 13, 10, 0
MSG_DISK_ERROR db "Error loading kernel!", 13, 10, 0

; Padding and magic number
times 510-($-$$) db 0
dw 0xaa55

