#include "trap.h"
#include "uart.h"
#include "panic.h"
#include <stdint.h>

// CLINT memory-mapped registers
#define CLINT_BASE 0x2000000UL
#define CLINT_MTIMECMP(hartid) (CLINT_BASE + 0x4000 + (hartid * 8))
#define CLINT_MTIME (CLINT_BASE + 0xBFF8)

// Function to set the next timer interrupt
void set_next_timer_interrupt(uint64_t hartid) {
    volatile uint64_t *mtime = (volatile uint64_t *)CLINT_MTIME;
    volatile uint64_t *mtimecmp = (volatile uint64_t *)CLINT_MTIMECMP(hartid);
    *mtimecmp = *mtime + 1000000UL;
}

// The C trap handler function for Supervisor mode.
void trap_handler(struct TrapFrame *frame) {
    uart_puts("=== Supervisor Mode Trap Occurred ===\n");
    uart_puts("scause: ");
    uart_puts_hex(frame->scause);
    uart_puts("\nsepc: ");
    uart_puts_hex(frame->sepc);
    uart_puts("\n");

    uint64_t is_interrupt = (frame->scause >> 63) & 1;
    uint64_t cause_code = frame->scause & ~(1UL << 63);

    if (is_interrupt) {
        if (cause_code == 5) { // Supervisor Timer Interrupt
            uart_puts("Handling Timer Interrupt...\n");
            set_next_timer_interrupt(0);
        } else {
            uart_puts("Unhandled interrupt in S-mode. scause=");
            uart_puts_hex(frame->scause);
            uart_puts("\n");
            panic("Unhandled Supervisor Interrupt");
        }
    } else {
        if (cause_code == 3) { // Breakpoint exception
            uart_puts("Breakpoint Exception caught.\n");
            frame->sepc += 2;
        } else if (cause_code == 0) { // Instruction Address Misaligned
            uart_puts("Instruction Address Misaligned Exception!\n");
            if (frame->sepc == 0) {
                panic("SEPC is 0 in misaligned exception");
            }
            frame->sepc += 4;
        } else {
            uart_puts("Unhandled Supervisor Exception!\n");
            uart_puts("scause=");
            uart_puts_hex(frame->scause);
            uart_puts("\nsepc=");
            uart_puts_hex(frame->sepc);
            uart_puts("\nstval=");
            uart_puts_hex(frame->stval);
            uart_puts("\n");
            panic("Unhandled Supervisor Exception");
        }
    }
}

// The C trap handler function for Machine mode.
void mtrap_handler(struct MTrapFrame *frame) {
    uart_puts("=== Machine Mode Trap Occurred ===\n");
    uart_puts("mcause: ");
    uart_puts_hex(frame->mcause);
    uart_puts("\nmepc: ");
    uart_puts_hex(frame->mepc);
    uart_puts("\n");

    uint64_t is_interrupt = (frame->mcause >> 63) & 1;
    uint64_t cause_code = frame->mcause & ~(1UL << 63);

    if (is_interrupt) {
        if (cause_code == 7) { // Machine Timer Interrupt
            uart_puts("Handling Machine Timer Interrupt...\n");
            set_next_timer_interrupt(0);
        } else {
            uart_puts("Unhandled interrupt in M-mode. mcause=");
            uart_puts_hex(frame->mcause);
            uart_puts("\n");
            panic("Unhandled Machine Interrupt");
        }
    } else {
        if (cause_code == 3) { // Breakpoint exception
            uart_puts("Breakpoint Exception in Machine Mode.\n");
            frame->mepc += 2;
        } else if (cause_code == 0) { // Instruction Address Misaligned
            uart_puts("Instruction Address Misaligned Exception in M-mode!\n");
            if (frame->mepc == 0) {
                panic("MEPC is 0 in misaligned exception");
            }
            frame->mepc += 4;
        } else {
            uart_puts("Unhandled Machine Exception!\n");
            uart_puts("mcause=");
            uart_puts_hex(frame->mcause);
            uart_puts("\nmepc=");
            uart_puts_hex(frame->mepc);
            uart_puts("\nmtval=");
            uart_puts_hex(frame->mtval);
            uart_puts("\n");
            panic("Unhandled Machine Exception");
        }
    }
}
