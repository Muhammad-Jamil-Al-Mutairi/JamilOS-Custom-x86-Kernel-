#include <stdint.h>
#include <stddef.h>
#include "io.h" 

// --- External Links (Functions located in other files) ---
extern void print_string(const char* str);
extern void clear_screen(void);
extern uint16_t* terminal_buffer; 
extern void start_multitasking(void); // Linked from kernel.c

// --- Helper Functions ---
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

// --- The TUI Window Generator ---
void draw_window(int start_x, int start_y, int width, int height, const char* title) {
    uint8_t window_color = (1 << 4) | 15; // Blue background, White text
    
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            size_t index = (y * 80) + x;
            char c = ' '; 
            
            if (y == start_y || y == start_y + height - 1) c = '-'; 
            if (x == start_x || x == start_x + width - 1) c = '|';  
            
            if ((y == start_y && x == start_x) || 
                (y == start_y && x == start_x + width - 1) ||
                (y == start_y + height - 1 && x == start_x) ||
                (y == start_y + height - 1 && x == start_x + width - 1)) {
                c = '+';
            }
            terminal_buffer[index] = (uint16_t) c | (uint16_t) window_color << 8;
        }
    }
    
    int title_x = start_x + (width / 2) - 4; 
    for(int i = 0; title[i] != '\0'; i++) {
         size_t index = (start_y * 80) + (title_x + i);
         terminal_buffer[index] = (uint16_t) title[i] | (uint16_t) window_color << 8;
    }
}

// --- The Main Command Evaluator ---
void execute_command(char* cmd) {
    if (cmd[0] == '\0') return; 

    // 1. HELP
    if (strcmp(cmd, "help") == 0) {
        print_string("OSJamil Commands:\n");
        print_string("1. help     2. clear    3. ipconfig  4. version   5. whoami\n");
        print_string("6. sysinfo  7. cpuinfo  8. meminfo   9. ping      10. ls\n");
        print_string("11. date    12. calc    13. echo     14. reboot   15. shutdown\n");
        print_string("16. serial  17. window  18. tasks\n");
    } 
    // 2. CLEAR
    else if (strcmp(cmd, "clear") == 0) {
        clear_screen();
    }
    // 3. IPCONFIG
    else if (strcmp(cmd, "ipconfig") == 0) {
        print_string("OSJamil Network Configuration:\n");
        print_string("   IPv4 Address. . . . . : 192.168.1.15\n");
        print_string("   Subnet Mask . . . . . : 255.255.255.0\n");
        print_string("   Default Gateway . . . : 192.168.1.1\n");
    }
    // 4. VERSION
    else if (strcmp(cmd, "version") == 0) {
        print_string("OSJamil Core Version 1.0 (x86 Custom Kernel)\n");
    }
    // 5. WHOAMI
    else if (strcmp(cmd, "whoami") == 0) {
        print_string("root_admin\n");
    }
    // 6. SYSINFO
    else if (strcmp(cmd, "sysinfo") == 0) {
        print_string("OS: OSJamil\nArchitecture: 32-bit x86\nKernel: Bare-metal C\n");
    }
    // 7. CPUINFO
    else if (strcmp(cmd, "cpuinfo") == 0) {
        print_string("CPU: Generic x86 Processor (QEMU Emulated)\n");
    }
    // 8. MEMINFO
    else if (strcmp(cmd, "meminfo") == 0) {
        print_string("RAM: 16 MB Total / 2 MB Kernel Used / 14 MB Free\n");
    }
    // 9. PING
    else if (strcmp(cmd, "ping") == 0) {
        print_string("Pinging 8.8.8.8 with 32 bytes of data:\nReply from 8.8.8.8: time<1ms\n");
    }
    // 10. LS
    else if (strcmp(cmd, "ls") == 0) {
        print_string("Volume in drive C is JAMIL_OS\n");
        print_string("kernel.bin    boot.asm    system32/   aistick_data/\n");
    }
    // 11. DATE
    else if (strcmp(cmd, "date") == 0) {
        print_string("Current Date: System Clock Active\n");
    }
    // 12. CALC
    else if (strcmp(cmd, "calc") == 0) {
        print_string("Error: Calculator GUI subsystem not initialized.\n");
    }
    // 13. ECHO
    else if (strncmp(cmd, "echo ", 5) == 0) {
        print_string(cmd + 5); 
        print_string("\n");
    }
    // 14. REBOOT (Hardware Portal)
    else if (strcmp(cmd, "reboot") == 0) {
        print_string("Opening Keyboard Controller Portal (0x64)...\nRestarting CPU...\n");
        outb(0x64, 0xFE); 
    }
    // 15. SHUTDOWN (Hardware Portal)
    else if (strcmp(cmd, "shutdown") == 0) {
        print_string("Opening ACPI Portal (0xF4)...\nShutting down OSJamil...\n");
        outb(0xf4, 0x00); 
    }
    // 16. SERIAL (Hardware Portal)
    else if (strcmp(cmd, "serial") == 0) {
        print_string("Opening COM1 Portal (0x3F8)...\nSending data to host...\n");
        const char* msg = "JamilOS has successfully transmitted data through COM1!\n";
        for (int i = 0; msg[i] != '\0'; i++) outb(0x3F8, msg[i]);
    }
    // 17. WINDOW (TUI)
    else if (strcmp(cmd, "window") == 0) {
        draw_window(20, 5, 40, 10, " SYSTEM ");
        print_string("\n\n\n\n\n\n\n\n\n\n\n"); 
    }
    // 18. TASKS (Multitasking Trigger)
    else if (strcmp(cmd, "tasks") == 0) {
        start_multitasking();
    }
    // ERROR HANDLING
    else {
        print_string("OSJamil Error: The command '");
        print_string(cmd);
        print_string("' is incorrect. Type 'help' for a list of commands.\n");
    }
}
