#include "proc.h"
#include "mem.h"
#include "vm.h"
#include "uart.h"
#include "context.h" // To reference struct context if needed.
#include <stddef.h>
#include <stdint.h>

// Maximum number of processes.
#define NPROC 64

// Fixed virtual address for mapping the user process's entry point.
#define USER_VA 0x1000

// Array of process control blocks.
static struct proc procs[NPROC];

// Pointer to the currently running process.
struct proc *current_proc = NULL;

// Scheduler context – used for context switching back to the scheduler.
struct context scheduler_context;

/*
 * Initialize the process table.
 * Set each PCB's state to UNUSED.
 */
void proc_init(void) {
    for (int i = 0; i < NPROC; i++) {
        procs[i].state = UNUSED;
    }
}

/*
 * Create a user process with the given entry point.
 * It sets up the kernel stack, user pagetable, maps the user code,
 * and initializes the process trap frame.
 */
void proc_create_user(void (*entry_point)(void)) {
    // Find an unused process slot.
    for (int i = 0; i < NPROC; i++) {
        if (procs[i].state == UNUSED) {
            procs[i].pid = i;
            procs[i].state = RUNNABLE;
            
            // Allocate a kernel stack for the process.
            procs[i].kstack = pfa_alloc();
            if (!procs[i].kstack) {
                uart_puts("Failed to allocate kernel stack for process.\n");
                return;
            }
            
            // Create a new user page table.
            procs[i].pagetable = vm_create_pagetable();
            if (!procs[i].pagetable) {
                uart_puts("Failed to create user pagetable.\n");
                return;
            }
            
            // Map the user program's code.
            // For simplicity, assume the process code fits in one page.
            vm_map(procs[i].pagetable, USER_VA, (uint64_t)entry_point, PTE_R | PTE_X);
            
            // Map the UART device into the user page table so the user process can print.
            vm_map(procs[i].pagetable, 0x10000000, 0x10000000, PTE_R | PTE_W);
            
            // Initialize the process's trap frame.
            procs[i].tf.sepc = USER_VA; // Start execution at the user program.
            
            // Clear register state in trap frame.
            for (int j = 0; j < 31; j++) {
                procs[i].tf.regs[j] = 0;
            }
            procs[i].tf.scause = 0;
            procs[i].tf.stval = 0;
            
            return;
        }
    }
    uart_puts("No available process slot!\n");
}

/*
 * The scheduler function.
 * Iterates through the process table and context switches to RUNNABLE processes.
 */
void scheduler(void) {
    while (1) {
        // Iterate over process table.
        for (int i = 0; i < NPROC; i++) {
            if (procs[i].state == RUNNABLE) {
                current_proc = &procs[i];
                current_proc->state = RUNNING;
                
                // Switch context from scheduler to process.
                // swtch will save scheduler_context and load current_proc->context.
                extern void swtch(struct context **old, struct context *new);
                swtch(&scheduler_context, &current_proc->context);
                
                // When the user process yields, execution resumes here.
                // It might have changed state (e.g., back to RUNNABLE).
            }
        }
    }
}

/*
 * Yield the CPU from the current process.
 * Mark the process as RUNNABLE and switch back to the scheduler.
 */
void yield(void) {
    if (current_proc) {
        current_proc->state = RUNNABLE;
        extern void swtch(struct context **old, struct context *new);
        swtch(&current_proc->context, &scheduler_context);
    }
}
