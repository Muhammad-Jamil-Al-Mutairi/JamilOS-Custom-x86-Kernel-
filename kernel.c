#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "gdt.h"
#include "idt.h"

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_LIGHT_GREY = 7,
};

/* Combine the character and the color into one 16-bit value */
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

void kernel_main(void) {
	/* The VGA text buffer is located at physical memory address 0xB8000 */
	init_gdt();
	init_idt();

	uint16_t* terminal_buffer = (uint16_t*) 0xB8000;
	
	/* Our text string */
	const char* str = "Hello, JamilOS World! My custom kernel is alive!";
	uint8_t color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
	
	/* Loop through the string and write it to the video memory */
	for (size_t i = 0; str[i] != '\0'; i++) {
		terminal_buffer[i] = vga_entry(str[i], color);
	}
	
	/* 2. Enable CPU interrupts AFTER the text is done printing */
        asm volatile("sti");

        /* 3. Hang the kernel here forever so it can listen for interrupts! */
        while(1) {
             // Do nothing, just wait for a key press
        }
}
