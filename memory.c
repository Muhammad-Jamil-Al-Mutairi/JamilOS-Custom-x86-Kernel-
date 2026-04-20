#include <stdint.h>
#include <stddef.h>

/* We will start allocating physical RAM from the 2 Megabyte mark (0x200000).
   This is safe because our kernel is loaded at 1 Megabyte! */
uint32_t free_memory_pointer = 0x200000;

/* Our custom Kernel Malloc function! */
void* kmalloc(size_t size) {
    /* Step 1: Grab the current free address to give to the program */
    void* allocated_memory = (void*) free_memory_pointer;
    
    /* Step 2: "Bump" the pointer forward by the requested size so the next 
               program doesn't overwrite this one. */
    free_memory_pointer += size;
    
    /* Step 3: Hand the memory over! */
    return allocated_memory;
}
