# JamilOS - Custom x86 Kernel

JamilOS is a custom, bare-metal 32-bit x86 Operating System built in C and Assembly. It runs completely independent of standard C libraries and interacts directly with system hardware using custom I/O port mapping.

## 🚀 Core Features
* **Custom Bootloader:** Multiboot-compliant assembly boot sequence.
* **Memory Management:** Custom Global Descriptor Table (GDT) and memory allocation.
* **Interrupt Handling:** Reprogrammed PIC and custom Interrupt Descriptor Table (IDT).
* **Keyboard Driver:** Hardware interrupt-driven keyboard input (IRQ 1).
* **VGA Text User Interface (TUI):** Custom screen buffering, dynamic window rendering, and color manipulation.

## 💻 Command Line Interface
The OS features a fully buffered command-line shell with 16 internal commands:
* `help`, `clear`, `version`, `whoami`, `sysinfo`, `cpuinfo`, `meminfo`, `date`, `calc`, `ping`, `ls`, `ipconfig`
* `echo` (Dynamic string buffering)
* `window` (Renders a dynamic TUI graphical window overlay)

## 🔌 Hardware Portals (I/O Ports)
JamilOS bypasses standard emulation by communicating directly with hardware I/O ports using inline assembly (`inb` / `outb`):
* **Keyboard Controller (0x60):** Reads raw scancodes from the keyboard buffer.
* **COM1 Serial Port (0x3F8):** Transmits debug data directly to the host machine.
* **ACPI Power (0xF4):** The `shutdown` command triggers a hardware power-off.
* **System Reset (0x64):** The `reboot` command triggers the CPU reset line.

## 🛠️ Build Instructions
To compile and emulate JamilOS using QEMU in a Linux/WSL environment:
1. Ensure `i686-elf-gcc` and `qemu-system-i386` are installed.
2. Run the master compilation script to assemble, compile, and link the kernel.
3. Boot the `.bin` file in QEMU.
