#include "process.h"
#include <stddef.h>

// --- 1. CONSTANTS ---
#define MAX_PROCESSES 16

// --- 2. GLOBAL VARIABLES ---
// These must be at the very top so all functions below can see them!
process_t process_table[MAX_PROCESSES];
uint32_t process_stacks[MAX_PROCESSES][1024]; // 4KB stack per process
process_t* current_process = NULL;
uint32_t next_pid = 1;


// --- 3. FUNCTIONS ---

// Initializes the table when the OS boots
void init_process_manager(void) {
    // Loop through the table and mark every slot as empty (BLOCKED)
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].state = PROCESS_STATE_BLOCKED;
        process_table[i].pid = 0;
    }
}

// Creates a new process and gives it a fake hardware state
void create_process(void (*entry_point)()) {
    // 1. Find an empty (BLOCKED) slot in the table
    int pid = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].state == PROCESS_STATE_BLOCKED) {
            pid = i;
            break;
        }
    }
    if (pid == -1) return; // The table is full!

    // 2. Set up the stack (Stacks grow downwards in x86, so start at the top)
    uint32_t* stack = &process_stacks[pid][1024];

    // 3. FAKE THE HARDWARE INTERRUPT FRAME (For the 'iret' instruction)
    *(--stack) = 0x0202;                // EFLAGS (Tells CPU to keep interrupts enabled)
    *(--stack) = 0x08;                  // CS (Kernel Code Segment)
    *(--stack) = (uint32_t)entry_point; // EIP (The actual function we want to run)

    // 4. FAKE THE PUSHAL FRAME (For the 'popal' instruction)
    *(--stack) = 0; // EDI
    *(--stack) = 0; // ESI
    *(--stack) = 0; // EBP
    *(--stack) = 0; // ESP (Ignored by popal, but needed for alignment)
    *(--stack) = 0; // EBX
    *(--stack) = 0; // EDX
    *(--stack) = 0; // ECX
    *(--stack) = 0; // EAX

    // 5. Save this fake stack pointer to the PCB and mark it READY
    process_table[pid].esp = (uint32_t)stack;
    process_table[pid].state = PROCESS_STATE_READY;
    process_table[pid].pid = next_pid++;
}

// The Round-Robin Scheduler
// The Round-Robin Scheduler
uint32_t schedule(uint32_t current_esp) {
    // --- THE COLD START FIX ---
    if (current_process == NULL) {
        // Find the very first READY process to kickstart the OS
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (process_table[i].state == PROCESS_STATE_READY) {
                current_process = &process_table[i];
                current_process->state = PROCESS_STATE_RUNNING;
                return current_process->esp; // Launch Task A!
            }
        }
        // If absolutely no programs exist, just return to the idle loop
        return current_esp;
    }

    // 1. Save the exact stack pointer where the current process was interrupted
    current_process->esp = current_esp;
    
    // 2. Put the current process back in line
    if (current_process->state == PROCESS_STATE_RUNNING) {
        current_process->state = PROCESS_STATE_READY;
    }

    // 3. Find the NEXT process in the table to run
    int current_index = current_process - process_table; 
    int next_index = current_index;

    // Loop through the table looking for a READY process
    do {
        next_index++;
        
        // If we hit the end of the table, wrap around to the beginning
        if (next_index >= MAX_PROCESSES) {
            next_index = 0; 
        }
        
        // If we found a process waiting for its turn, stop searching!
        if (process_table[next_index].state == PROCESS_STATE_READY) {
            break;
        }
    } while (next_index != current_index);

    // 4. Set the new process as the currently running one
    current_process = &process_table[next_index];
    current_process->state = PROCESS_STATE_RUNNING;

    // 5. Hand the CPU the "controller" (the new stack pointer)
    return current_process->esp;
}
