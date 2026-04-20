.global gdt_flush
.type gdt_flush, @function

gdt_flush:
    /* Get the pointer to the GDT, passed as a parameter from C */
    mov 4(%esp), %eax
    
    /* Load it into the CPU! */
    lgdt (%eax)

    /* Load the data segment registers with our new Data Segment offset (0x10) */
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    /* Do a 'far jump' to update the Code Segment (0x08) */
    jmp $0x08, $.flush
.flush:
    ret

