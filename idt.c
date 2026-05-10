#include "idt.h"
#include "io.h"

// Create an array of 256 IDT entries
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

// External assembly function to load the IDT
extern void idt_flush(uint32_t);
extern void keyboard_handler_stub(void);

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    // We must uncomment the OR below when we get to user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt_entries[num].flags   = flags /* | 0x60 */;
}

void init_idt(void) {
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    // Clear out the entire IDT, initializing it to zeros
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    /* Remap the PIC so hardware interrupts don't crash the CPU */
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    // Link the new assembly stub we just made
    extern void timer_handler_stub(void);

    /* Mask interrupts EXCEPT Timer (IRQ 0) and Keyboard (IRQ 1) */
    // 0xFC in binary is 11111100 (Bits 0 and 1 are unmasked!)
    outb(0x21, 0xFC); outb(0xA1, 0xFF);

    /* Hook our timer stub to interrupt 32 */
    idt_set_gate(32, (uint32_t)timer_handler_stub, 0x08, 0x8E);
    /* Hook our keyboard stub to interrupt 33 */
    idt_set_gate(33, (uint32_t)keyboard_handler_stub, 0x08, 0x8E);

    

    // Tell the CPU where to find the IDT
    idt_flush((uint32_t)&idt_ptr);
}

