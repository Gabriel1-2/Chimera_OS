#ifndef TRAP_H
#define TRAP_H

#include <stdint.h>

// Structure to save general-purpose registers and Supervisor-level CSRs on the stack during a trap.
// This must exactly match the order in which registers are saved in trap.S for Supervisor mode.
struct TrapFrame {
    uint64_t regs[31]; // x1 through x31
    uint64_t sepc;     // Supervisor Exception Program Counter
    uint64_t scause;   // Supervisor Cause Register
    uint64_t stval;    // Supervisor Trap Value Register
};

// Structure to save general-purpose registers and Machine-level CSRs on the stack during a trap.
// This must exactly match the order in which registers are saved in trap.S for Machine mode.
struct MTrapFrame {
    uint64_t regs[31]; // x1 through x31
    uint64_t mepc;     // Machine Exception Program Counter
    uint64_t mcause;   // Machine Cause Register
    uint64_t mtval;    // Machine Trap Value Register
};

// The C trap handler function for Supervisor mode.
// It receives a pointer to the saved TrapFrame on the stack.
void trap_handler(struct TrapFrame *frame);

// The C trap handler function for Machine mode.
// It receives a pointer to the saved MTrapFrame on the stack.
void mtrap_handler(struct MTrapFrame *frame);

#endif // TRAP_H
