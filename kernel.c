#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "process.h"

extern void print_string(const char* str);
extern void clear_screen(void);

// --- OUR BACKGROUND PROCESSES ---
void task_a() {
    while(1) {
        print_string("[Process One] ");
        // A larger delay so it doesn't instantly flood the terminal
        for(volatile int i=0; i<5000000; i++); 
    }
}

void task_b() {
    while(1) {
        print_string("[Process Two] ");
        for(volatile int i=0; i<5000000; i++); 
    }
}

// A function for the shell to call when you type the new command
void start_multitasking(void) {
    print_string("\nDeploying Task A and Task B to the CPU...\n");
    create_process(task_a);
    create_process(task_b);
}

void kernel_main(void) {
    init_gdt();
    init_idt();
    
    // Set up the multitasking table, but DO NOT start the tasks yet!
    init_process_manager(); 
    init_timer(100); 

    // Restore our clean terminal boot
    clear_screen();
    print_string("Welcome to OSJamil Terminal!\n");
    print_string("Type 'help' to see available commands.\n\n");
    print_string("OSJamil> ");

    // Turn on the CPU Interrupts so the keyboard and clock work
    asm volatile("sti");

    while(1) {
        // The main kernel idles here waiting for you to type commands
    }
}

