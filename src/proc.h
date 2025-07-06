#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include "vm.h"

/* Process States */
enum proc_state {
    UNUSED = 0,
    SLEEPING,
    RUNNABLE,
    RUNNING
};

/* 
 * Copy of the TrapFrame definition from src/trap.h.
 * Each process will have its own trap frame to store user-state.
 */
struct TrapFrame {
    uint64_t regs[31]; // x1 through x31.
    uint64_t sepc;     // Supervisor Exception Program Counter.
    uint64_t scause;   // Supervisor Cause Register.
    uint64_t stval;    // Supervisor Trap Value Register.
};

/* The Process Control Block (PCB) keeping track of a process. */
struct proc {
    uint64_t pid;                  // Unique process ID.
    enum proc_state state;         // Process state.
    void *kstack;                  // Pointer to the top of the kernel stack.
    pagetable_t pagetable;         // Pointer to the user-space pagetable.
    struct TrapFrame tf;           // Process trap frame (user registers).
    struct context {               // Context for switching (callee-saved registers).
        uint64_t ra;
        uint64_t s0;
        uint64_t s1;
        uint64_t s2;
        uint64_t s3;
        uint64_t s4;
        uint64_t s5;
        uint64_t s6;
        uint64_t s7;
        uint64_t s8;
        uint64_t s9;
        uint64_t s10;
        uint64_t s11;
    } context;
};

#endif // PROC_H
