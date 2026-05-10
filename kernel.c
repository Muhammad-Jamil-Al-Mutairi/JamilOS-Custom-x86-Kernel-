#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "process.h"

extern void print_string(const char* str);
extern void clear_screen(void);

// --- OUR FIRST BACKGROUND PROCESS ---
void task_a() {
    while(1) {
        print_string("A");
        // A small delay so it doesn't flood the screen instantly
        for(volatile int i=0; i<1000000; i++); 
    }
}

// --- OUR SECOND BACKGROUND PROCESS ---
void task_b() {
    while(1) {
        print_string("B");
        // A small delay
        for(volatile int i=0; i<1000000; i++); 
    }
}

void kernel_main(void) {
    init_gdt();
    init_idt();
    
    clear_screen();
    print_string("OSJamil Kernel Booting...\n");
    print_string("Starting Multitasking Scheduler...\n\n");

    init_process_manager(); 
    
    // Create our two background tasks!
    create_process(task_a);
    create_process(task_b);

    // Turn on the CPU Interrupts
    asm volatile("sti");

    // Start the heartbeat! The CPU will now begin ripping control 
    // away from the idle loop and switching between Task A and Task B!
    init_timer(100); 

    while(1) {
        // The main kernel idles here forever while the background tasks run.
    }
}
