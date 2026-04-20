#include <stdint.h>
#include <stddef.h>
#include "io.h"

/* Standard US Keyboard scancode to ASCII map */
const char kbdus[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

size_t terminal_row = 1;
size_t terminal_column = 0;

void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);

    if (!(scancode & 0x80)) {
        char c = kbdus[scancode];
        uint16_t* terminal_buffer = (uint16_t*) 0xB8000;

        /* --- NEW BACKSPACE LOGIC --- */
        if (c == '\b') {
            /* 1. Move cursor back one space */
            if (terminal_column > 0) {
                terminal_column--;
            } else if (terminal_row > 1) { 
                /* Wrap back to the previous line, but protect row 0 (our welcome message!) */
                terminal_row--;
                terminal_column = 79;
            }
            
            /* 2. Erase the character by printing a blank space over it */
            size_t index = (terminal_row * 80) + terminal_column;
            terminal_buffer[index] = (uint16_t) ' ' | (uint16_t) 15 << 8;
            
        } 
        /* --- NORMAL PRINTING LOGIC --- */
        else if (c != 0) {
            size_t index = (terminal_row * 80) + terminal_column;
            terminal_buffer[index] = (uint16_t) c | (uint16_t) 15 << 8;
            
            terminal_column++;
            if (terminal_column >= 80) {
                terminal_column = 0;
                terminal_row++;
            }
        }
    }
    
    outb(0x20, 0x20);
}
