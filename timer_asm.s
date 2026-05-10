.global timer_handler_stub
.type timer_handler_stub, @function

timer_handler_stub:
    /* 1. Save the state of the current program (Process A) */
    pushal              

    /* 2. Put our current Stack Pointer (ESP) into EAX so we can pass it to C */
    mov %esp, %eax      
    
    /* 3. Push EAX onto the stack as the argument for our C function */
    push %eax           
    
    /* 4. Call the C timer handler (which will call the scheduler) */
    call timer_handler  
    
    /* 5. THE MAGIC: timer_handler returned Process B's stack pointer in EAX.
          We now completely overwrite the CPU's stack pointer! */
    mov %eax, %esp      
    
    /* 6. Restore the state of the new program (Process B) */
    popal               
    
    /* 7. Resume execution inside Process B */
    iret
