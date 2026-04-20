.global idt_flush
.type idt_flush, @function

idt_flush:
    /* Get the pointer to the IDT, passed as a parameter from C */
    mov 4(%esp), %eax
    
    /* Load the IDT pointer into the CPU! */
    lidt (%eax)
    ret

