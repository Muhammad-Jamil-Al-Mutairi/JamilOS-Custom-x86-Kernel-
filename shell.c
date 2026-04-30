#include <stdint.h>
#include <stddef.h>
#include "io.h" 

extern void print_string(const char* str);
extern void clear_screen(void);
extern uint16_t* terminal_buffer; // We need direct access to the screen memory

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

// --- NEW CODE: The TUI Window Generator ---
void draw_window(int start_x, int start_y, int width, int height, const char* title) {
    // Color byte: Background Blue (1) << 4 | Foreground White (15)
    uint8_t window_color = (1 << 4) | 15; 
    
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            size_t index = (y * 80) + x;
            char c = ' '; // Default fill character
            
            // Draw Borders
            if (y == start_y || y == start_y + height - 1) c = '-'; // Top/Bottom
            if (x == start_x || x == start_x + width - 1) c = '|';  // Sides
            
            // Draw Corners
            if ((y == start_y && x == start_x) || 
                (y == start_y && x == start_x + width - 1) ||
                (y == start_y + height - 1 && x == start_x) ||
                (y == start_y + height - 1 && x == start_x + width - 1)) {
                c = '+';
            }

            // Write the character and color to video memory
            terminal_buffer[index] = (uint16_t) c | (uint16_t) window_color << 8;
        }
    }
    
    // Draw the Title at the top center of the window
    int title_x = start_x + (width / 2) - 4; // approximate centering
    for(int i = 0; title[i] != '\0'; i++) {
         size_t index = (start_y * 80) + (title_x + i);
         terminal_buffer[index] = (uint16_t) title[i] | (uint16_t) window_color << 8;
    }
}
// ------------------------------------------

void execute_command(char* cmd) {
    if (cmd[0] == '\0') return; 

    if (strcmp(cmd, "help") == 0) {
        print_string("OSJamil Commands:\n");
        print_string("1. help     2. clear    3. ipconfig  4. version   5. whoami\n");
        print_string("6. sysinfo  7. cpuinfo  8. meminfo   9. ping      10. ls\n");
        print_string("11. date    12. calc    13. echo     14. reboot   15. shutdown\n");
        print_string("16. window (Opens TUI)\n");
    } 
    // ... (Keep all your other 15 commands here exactly as they were) ...
    
    // 16. THE WINDOW COMMAND
    else if (strcmp(cmd, "window") == 0) {
        // Draw a window at X=20, Y=5, Width=40, Height=10
        draw_window(20, 5, 40, 10, " SYSTEM ");
        
        // Move the cursor down so it doesn't type inside the window
        print_string("\n\n\n\n\n\n\n\n\n\n\n"); 
    }
    else {
        print_string("OSJamil Error: The command '");
        print_string(cmd);
        print_string("' is incorrect.\n");
    }
}
