#include "uart.h"
#include "mem.h"
#include "trap.h"
#include "vm.h"
#include "proc.h"      // Include process management.
#include <stdint.h>

// Externally defined trap vector from trap.S.
extern void __trap_vector(void);
// Externally defined user process.
extern void user_process_1(void);

void kmain(uint64_t hartid, uint64_t dtb_paddr) {
    uart_puts("Chimera OS: kmain entered.\n");
    uart_puts("Hart ID: ");
    uart_puts_hex(hartid);
    uart_puts("\n");
    uart_puts("DTB Physical Address: ");
    uart_puts_hex(dtb_paddr);
    uart_puts("\n");

    // Install trap vector.
    asm volatile("csrw stvec, %0" : : "r"((uint64_t)__trap_vector));
    uart_puts("Trap vector installed.\n");

    // Initialize timer interrupts.
    // (Timer interrupt setup remains from previous code.)
    {
        volatile uint64_t *mtime = (volatile uint64_t *)0x200BFF8;
        volatile uint64_t *mtimecmp = (volatile uint64_t *)(0x2004000 + (0 * 8));
        *mtimecmp = *mtime + 1000000ULL;
        
        uint64_t sie;
        asm volatile("csrr %0, sie" : "=r"(sie));
        sie |= (1 << 5);
        asm volatile("csrw sie, %0" :: "r"(sie));
        
        uint64_t sstatus;
        asm volatile("csrr %0, sstatus" : "=r"(sstatus));
        sstatus |= (1 << 1);
        asm volatile("csrw sstatus, %0" :: "r"(sstatus));
    }

    // Enable virtual memory.
    // (Assume enable_virtual_memory() is defined in kernel.c or another file, as before.)
    extern void enable_virtual_memory(void);
    enable_virtual_memory();

    // Initialize process table.
    proc_init();

    // Create our first user process.
    proc_create_user(user_process_1);

    // Start scheduler loop.
    scheduler();

    // Should never reach here.
    while (1) {}
}
