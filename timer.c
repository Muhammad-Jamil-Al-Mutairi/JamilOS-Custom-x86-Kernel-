#include <stdint.h>
#include "timer.h"
#include "io.h"
#include <stddef.h>

// A global counter to track how many times the clock has ticked
uint32_t tick = 0;


// This function is what the CPU will execute every single time the clock ticks!
extern uint32_t schedule(uint32_t current_esp);// Link to your print function

uint32_t timer_handler(uint32_t esp) {
    tick++;
    
    // Debug: Print a message every 100 ticks (1 second) to prove it works!
    //if (tick % 100 == 0) {
      //  print_string("[Heartbeat] ");
    //}

    // CRITICAL: Send End Of Interrupt (EOI) to the PIC

    outb(0x20, 0x20); 
    
    return schedule(esp);
}


void init_timer(uint32_t frequency) {
    // 1. The hardware math: The PIT oscillates at 1193180 Hz.
    // To get our desired frequency, we divide the base clock by our frequency.
    uint32_t divisor = 1193180 / frequency;

    // 2. Open the Command Portal (0x43)
    // Send byte 0x36, which tells the PIT: "Get ready, I am sending a new divisor!"
    outb(0x43, 0x36);

    // 3. Open the Data Portal (0x40)
    // The PIT can only receive 8 bits (1 byte) at a time, but our divisor is 16 bits.
    // So, we split the divisor in half and send it in two pieces:
    uint8_t low_byte = (uint8_t)(divisor & 0xFF);
    uint8_t high_byte = (uint8_t)((divisor >> 8) & 0xFF);

    outb(0x40, low_byte);
    outb(0x40, high_byte);
}
