#include <stdint.h>
#include <stddef.h>
#include "io.h"

const char kbdus[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

size_t terminal_row = 0;
size_t terminal_column = 0;
uint16_t* terminal_buffer = (uint16_t*) 0xB8000;

// Variables for the command line
char command_buffer[256];
int buffer_index = 0;

// Link to the shell file
extern void execute_command(char* cmd);

// A proper print function that handles newlines
void print_string(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            terminal_column = 0;
            terminal_row++;
        } else {
            size_t index = (terminal_row * 80) + terminal_column;
            terminal_buffer[index] = (uint16_t) str[i] | (uint16_t) 15 << 8;
            terminal_column++;
            if (terminal_column >= 80) {
                terminal_column = 0;
                terminal_row++;
            }
        }
    }
}

// Function to clear the VGA screen
void clear_screen() {
    for (size_t y = 0; y < 25; y++) {
        for (size_t x = 0; x < 80; x++) {
            size_t index = (y * 80) + x;
            terminal_buffer[index] = (uint16_t) ' ' | (uint16_t) 15 << 8;
        }
    }
    terminal_row = 0;
    terminal_column = 0;
}

void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);

    if (!(scancode & 0x80)) {
        char c = kbdus[scancode];

        if (c == '\n') { // Enter Key
            command_buffer[buffer_index] = '\0'; // Cap off the string
            print_string("\n"); // Move cursor to the next line
            execute_command(command_buffer); // Run the command
            
            // Reset buffer for the next command
            buffer_index = 0; 
            print_string("OSJamil> ");
        } 
        else if (c == '\b') { // Backspace Key
            if (buffer_index > 0) {
                buffer_index--; // Remove char from our logic buffer
                
                // Visually remove it from the screen
                if (terminal_column > 0) {
                    terminal_column--;
                } else if (terminal_row > 0) { 
                    terminal_row--;
                    terminal_column = 79;
                }
                size_t index = (terminal_row * 80) + terminal_column;
                terminal_buffer[index] = (uint16_t) ' ' | (uint16_t) 15 << 8;
            }
        } 
        else if (c != 0) { // Standard character
            if (buffer_index < 255) {
                command_buffer[buffer_index++] = c; // Add to buffer
            }
            
            // Visually print it
            size_t index = (terminal_row * 80) + terminal_column;
            terminal_buffer[index] = (uint16_t) c | (uint16_t) 15 << 8;
            terminal_column++;
            if (terminal_column >= 80) {
                terminal_column = 0;
                terminal_row++;
            }
        }
    }
    
    // Acknowledge the interrupt
    outb(0x20, 0x20);
}
