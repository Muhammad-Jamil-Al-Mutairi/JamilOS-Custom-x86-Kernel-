#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

// Define the three states a program can be in
#define PROCESS_STATE_READY   0   // Waiting for its turn
#define PROCESS_STATE_RUNNING 1   // Currently using the CPU
#define PROCESS_STATE_BLOCKED 2   // Paused or empty slot

// The Process Control Block (PCB)
typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi; // General purpose math/data registers
    uint32_t esp, ebp;                     // STACK POINTERS (The most important part!)
    uint32_t eip;                          // INSTRUCTION POINTER (Where the code paused)
    uint32_t eflags;                       // CPU status flags
    int state;                             // Is it READY, RUNNING, or BLOCKED?
    uint32_t pid;                          // Process ID (Just like Linux PIDs)
} process_t;

// Function to set up the system
void init_process_manager(void);
// The scheduler function: takes the old stack pointer, returns the new one

uint32_t schedule(uint32_t current_esp);
// Creates a new process and adds it to the scheduler queue
void create_process(void (*entry_point)());
#endif
