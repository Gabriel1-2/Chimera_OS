#include "trap.h"
#include "uart.h"
#include "panic.h"
#include "proc.h"  // Include process management to access yield().
#include <stdint.h>

#define TIMER_INTERRUPT_CODE 5UL  // Supervisor Timer Interrupt cause code

/*
 * Helper: Set the next timer interrupt.
 * Uses the CLINT registers.
 */
static void set_next_timer_interrupt(uint64_t hartid) {
    volatile uint64_t *mtime = (volatile uint64_t *)0x200BFF8;
    volatile uint64_t *mtimecmp = (volatile uint64_t *)(0x2004000 + hartid * 8);
    *mtimecmp = *mtime + 1000000ULL;
}

/*
 * C trap handler for Supervisor mode.
 * On a timer interrupt, it yields control back to the scheduler.
 */
void trap_handler(struct TrapFrame *frame) {
    uint64_t is_interrupt = (frame->scause >> 63) & 1;
    uint64_t cause_code = frame->scause & ~(1UL << 63);

    if (is_interrupt) {
        if (cause_code == TIMER_INTERRUPT_CODE) {
            uart_puts("Timer Interrupt! Yielding CPU...\n");
            set_next_timer_interrupt(0);
            yield();  // Call yield() to switch back to the scheduler.
        } else {
            uart_puts("Unexpected interrupt! scause=0x");
            uart_puts_hex(frame->scause);
            uart_puts("\n");
            panic("Unhandled Supervisor interrupt");
        }
    } else {
        uart_puts("Unexpected exception! scause=0x");
        uart_puts_hex(frame->scause);
        uart_puts("\n");
        panic("Unhandled Supervisor exception");
    }
    while(1) {} // Should never reach here.
}
