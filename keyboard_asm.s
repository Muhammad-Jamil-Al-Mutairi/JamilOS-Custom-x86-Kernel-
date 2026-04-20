.global keyboard_handler_stub
.type keyboard_handler_stub, @function

keyboard_handler_stub:
    pushal                  /* Save all CPU registers */
    call keyboard_handler   /* Jump to our C function! */
    popal                   /* Restore all CPU registers */
    iret                   /* Return from the interrupt safely */

