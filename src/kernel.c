#include "uart.h"
#include "mem.h"
#include "trap.h"
#include <stdint.h>

// CLINT memory-mapped registers
#define CLINT_BASE 0x2000000UL
#define CLINT_MTIMECMP(hartid) (CLINT_BASE + 0x4000 + (hartid * 8))
#define CLINT_MTIME (CLINT_BASE + 0xBFF8)

// Function to set the next timer interrupt (declared in trap.c, but needed here for direct call)
extern void set_next_timer_interrupt(uint64_t hartid);

// This function will be the entry point when we switch to Supervisor mode.
void _start_supervisor_c(uint64_t hartid, uint64_t dtb_paddr) {
    while(1) { /* Simplified S-mode entry */ }
}

// Declare the assembly function for mode switch
extern void switch_to_s_mode(uint64_t hartid, uint64_t dtb_paddr, uint64_t supervisor_entry_point);

void kmain(uint64_t hartid, uint64_t dtb_paddr) {
    uart_puts("Chimera: kmain entered (Machine mode).\n");

    // Report hart id and DTB address
    uart_puts("Hart ID: ");
    uart_puts_hex(hartid);
    uart_puts("\n");

    uart_puts("DTB Physical Address: ");
    uart_puts_hex(dtb_paddr);
    uart_puts("\n");

    // --- Prepare for Supervisor Mode Transition ---

    // Disable PMP for now to ensure full memory access in S-mode
    asm volatile("csrw pmpcfg0, zero");
    asm volatile("csrw pmpaddr0, zero");

    // Call the assembly function to switch to Supervisor mode
    extern void s_mode_stub(void);
    switch_to_s_mode(hartid, dtb_paddr, (uint64_t)s_mode_stub);

    // This code should not be reached after switch_to_s_mode
    uart_puts("Error: Returned from switch_to_s_mode in kmain (should not happen).\n");
    while(1) {}
}