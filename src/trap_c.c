#include "trap.h"
#include "uart.h"
#include <stdint.h>

// CLINT memory-mapped registers
#define CLINT_BASE 0x2000000UL
#define CLINT_MTIMECMP(hartid) (CLINT_BASE + 0x4000 + (hartid * 8))
#define CLINT_MTIME (CLINT_BASE + 0xBFF8)

// Function to set the next timer interrupt
void set_next_timer_interrupt(uint64_t hartid) {
    // Read current mtime
    volatile uint64_t *mtime = (volatile uint64_t *)CLINT_MTIME;
    volatile uint64_t *mtimecmp = (volatile uint64_t *)CLINT_MTIMECMP(hartid);

    // Set mtimecmp for the next interrupt
    // Add a large interval (e.g., 1,000,000 cycles)
    *mtimecmp = *mtime + 1000000UL;
}

// The C trap handler function for Supervisor mode.
void trap_handler(struct TrapFrame *frame) {
    uart_puts("Caught a trap from Supervisor mode!\n");
    uart_puts("  Raw scause: ");
    uart_puts_hex(frame->scause);
    uart_puts("\n");
    uart_puts("  SEPC (on entry): ");
    uart_puts_hex(frame->sepc);
    uart_puts("\n");

    // Determine if it's an interrupt or exception
    uint64_t is_interrupt = (frame->scause >> 63) & 1;
    uint64_t cause_code = frame->scause & (~(1UL << 63));

    if (is_interrupt) {
        // Handle interrupts
        if (cause_code == 5) { // Supervisor Timer Interrupt (cause code 5 for S-mode)
            uart_puts("Timer Interrupt!\n");
            // Re-arm the timer for the next interrupt
            set_next_timer_interrupt(0); // Assuming hart 0 for now
            // For interrupts, sepc should NOT be advanced. Execution resumes at the interrupted instruction.
        } else {
            uart_puts("Unexpected interrupt! scause=");
            uart_puts_hex(frame->scause);
            uart_puts("\n");
        }
    } else {
        // Handle exceptions
        if (cause_code == 3) { // Breakpoint exception
            uart_puts("Caught a breakpoint!\n");
            frame->sepc += 2; // Advance sepc past the 2-byte ebreak instruction
        } else if (cause_code == 0) { // Instruction Address Misaligned
            uart_puts("Instruction Address Misaligned!\n");
            // If sepc is 0, we can't advance it. This indicates a severe issue.
            // For now, just print and halt to avoid infinite traps from 0.
            if (frame->sepc == 0) {
                uart_puts("  SEPC is 0, halting.\n");
                while(1){}
            }
            // Otherwise, try to advance by 4 bytes (common instruction length)
            // This is a guess, as the actual misaligned instruction length is unknown.
            frame->sepc += 4;
        } else {
            uart_puts("Unexpected exception! scause=");
            uart_puts_hex(frame->scause);
            uart_puts("\n");
            uart_puts("  sepc=");
            uart_puts_hex(frame->sepc);
            uart_puts("\n");
            uart_puts("  stval=");
            uart_puts_hex(frame->stval);
            uart_puts("\n");
        }
    }
}

// The C trap handler function for Machine mode.
void mtrap_handler(struct MTrapFrame *frame) {
    uart_puts("Caught a trap from Machine mode!\n");
    uart_puts("  Raw mcause: ");
    uart_puts_hex(frame->mcause);
    uart_puts("\n");
    uart_puts("  MEPC (on entry): ");
    uart_puts_hex(frame->mepc);
    uart_puts("\n");

    // Determine if it's an interrupt or exception
    uint64_t is_interrupt = (frame->mcause >> 63) & 1;
    uint64_t cause_code = frame->mcause & (~(1UL << 63));

    if (is_interrupt) {
        // Handle interrupts
        if (cause_code == 7) { // Machine Timer Interrupt (cause code 7 for M-mode)
            uart_puts("Timer Interrupt (M-mode)!\n");
            // Re-arm the timer for the next interrupt
            set_next_timer_interrupt(0); // Assuming hart 0 for now
            // For interrupts, mepc should NOT be advanced. Execution resumes at the interrupted instruction.
        } else {
            uart_puts("Unexpected interrupt (M-mode)! mcause=");
            uart_puts_hex(frame->mcause);
            uart_puts("\n");
        }
    } else {
        // Handle exceptions
        if (cause_code == 3) { // Breakpoint exception
            uart_puts("Caught a breakpoint (M-mode)!\n");
            frame->mepc += 2; // Advance mepc past the 2-byte ebreak instruction
        } else if (cause_code == 0) { // Instruction Address Misaligned
            uart_puts("Instruction Address Misaligned (M-mode)!\n");
            // If mepc is 0, we can't advance it. This indicates a severe issue.
            // For now, just print and halt to avoid infinite traps from 0.
            if (frame->mepc == 0) {
                uart_puts("  MEPC is 0, halting.\n");
                while(1){}
            }
            // Otherwise, try to advance by 4 bytes (common instruction length)
            // This is a guess, as the actual misaligned instruction length is unknown.
            frame->mepc += 4;
        } else {
            uart_puts("Unexpected exception (M-mode)! mcause=");
            uart_puts_hex(frame->mcause);
            uart_puts("\n");
            uart_puts("  mepc=");
            uart_puts_hex(frame->mepc);
            uart_puts("\n");
            uart_puts("  mtval=");
            uart_puts_hex(frame->mtval);
            uart_puts("\n");
        }
    }
}
