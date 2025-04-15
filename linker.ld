ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
SECTIONS {
    . = 0x1000;    /* Load kernel at 0x1000 */
    
    .text BLOCK(4K) : ALIGN(4K) {
        *(.text.start)  /* Start with _start section */
        *(.text)        /* Then other text sections */
    }
    
    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
    }
    
    .data BLOCK(4K) : ALIGN(4K) {
        *(.data)
    }
    
    .bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }
    
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}
