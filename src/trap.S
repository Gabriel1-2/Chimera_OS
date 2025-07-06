#
# Project Chimera - RISC-V Trap Vectors
#
# This file defines assembly entry points for traps in different privilege modes.
#

.section .text

# --- Supervisor Mode Trap Vector ---
# This is the entry point for all traps (exceptions and interrupts)
# when the CPU is in Supervisor mode.
#
# It saves Supervisor-level CSRs (sepc, scause, stval) and general-purpose registers.
# It then calls the C-level trap_handler.
#
.global __trap_vector
__trap_vector:
    # Allocate space for the TrapFrame on the stack.
    # 31 GPRs (x1-x31) + 3 CSRs (sepc, scause, stval) = 34 * 8 bytes = 272 bytes.
    addi sp, sp, -272

    # Save general-purpose registers (x1-x31) onto the stack.
    # The order here must match the TrapFrame structure in trap.h.
    # x1 (ra) is at (0 * 8)(sp)
    sd x1,  (0 * 8)(sp)
    sd x2,  (1 * 8)(sp) # sp itself, will be restored from the original sp
    sd x3,  (2 * 8)(sp)
    sd x4,  (3 * 8)(sp)
    sd x5,  (4 * 8)(sp)
    sd x6,  (5 * 8)(sp)
    sd x7,  (6 * 8)(sp)
    sd x8,  (7 * 8)(sp)
    sd x9,  (8 * 8)(sp)
    sd x10, (9 * 8)(sp)
    sd x11, (10 * 8)(sp)
    sd x12, (11 * 8)(sp)
    sd x13, (12 * 8)(sp)
    sd x14, (13 * 8)(sp)
    sd x15, (14 * 8)(sp)
    sd x16, (15 * 8)(sp)
    sd x17, (16 * 8)(sp)
    sd x18, (17 * 8)(sp)
    sd x19, (18 * 8)(sp)
    sd x20, (19 * 8)(sp)
    sd x21, (20 * 8)(sp)
    sd x22, (21 * 8)(sp)
    sd x23, (22 * 8)(sp)
    sd x24, (23 * 8)(sp)
    sd x25, (24 * 8)(sp)
    sd x26, (25 * 8)(sp)
    sd x27, (26 * 8)(sp)
    sd x28, (27 * 8)(sp)
    sd x29, (28 * 8)(sp)
    sd x30, (29 * 8)(sp)
    sd x31, (30 * 8)(sp)

    # Save Supervisor-level CSRs onto the stack.
    # sepc is at (31 * 8)(sp)
    # scause is at (32 * 8)(sp)
    # stval is at (33 * 8)(sp)
    csrr t0, sepc
    sd t0, (31 * 8)(sp) # Save sepc
    csrr t0, scause
    sd t0, (32 * 8)(sp) # Save scause
    csrr t0, stval
    sd t0, (33 * 8)(sp) # Save stval

    # Call the C trap handler.
    # The first argument (a0) is the pointer to the saved TrapFrame (which is the current sp).
    mv a0, sp
    call trap_handler

    # Restore Supervisor-level CSRs from the stack.
    # Restore sepc last among CSRs, as it's critical for trap return.
    ld t0, (33 * 8)(sp) # Load stval
    csrw stval, t0
    ld t0, (32 * 8)(sp) # Load scause
    csrw scause, t0
    ld t0, (31 * 8)(sp) # Load sepc
    csrw sepc, t0

    # Restore general-purpose registers (x1-x31) from the stack.
    ld x1,  (0 * 8)(sp)
    ld x2,  (1 * 8)(sp)
    ld x3,  (2 * 8)(sp)
    ld x4,  (3 * 8)(sp)
    ld x5,  (4 * 8)(sp)
    ld x6,  (5 * 8)(sp)
    ld x7,  (6 * 8)(sp)
    ld x8,  (7 * 8)(sp)
    ld x9,  (8 * 8)(sp)
    ld x10, (9 * 8)(sp)
    ld x11, (10 * 8)(sp)
    ld x12, (11 * 8)(sp)
    ld x13, (12 * 8)(sp)
    ld x14, (13 * 8)(sp)
    ld x15, (14 * 8)(sp)
    ld x16, (15 * 8)(sp)
    ld x17, (16 * 8)(sp)
    ld x18, (17 * 8)(sp)
    ld x19, (18 * 8)(sp)
    ld x20, (19 * 8)(sp)
    ld x21, (20 * 8)(sp)
    ld x22, (21 * 8)(sp)
    ld x23, (22 * 8)(sp)
    ld x24, (23 * 8)(sp)
    ld x25, (24 * 8)(sp)
    ld x26, (25 * 8)(sp)
    ld x27, (26 * 8)(sp)
    ld x28, (27 * 8)(sp)
    ld x29, (28 * 8)(sp)
    ld x30, (29 * 8)(sp)
    ld x31, (30 * 8)(sp)

    # Deallocate space for the TrapFrame from the stack.
    addi sp, sp, 272

    # Return from trap using sret (Supervisor Return).
    # This restores the program counter (pc) from sepc and sets the privilege mode.
    sret

# --- Machine Mode Trap Vector ---
# This is the entry point for all traps (exceptions and interrupts)
# when the CPU is in Machine mode.
#
# It saves Machine-level CSRs (mepc, mcause, mtval) and general-purpose registers.
# It then calls the C-level mtrap_handler.
#
.global __mtrap_vector
__mtrap_vector:
    # Allocate space for the MTrapFrame on the stack.
    # 31 GPRs (x1-x31) + 3 CSRs (mepc, mcause, mtval) = 34 * 8 bytes = 272 bytes.
    addi sp, sp, -272

    # Save general-purpose registers (x1-x31) onto the stack.
    sd x1,  (0 * 8)(sp)
    sd x2,  (1 * 8)(sp)
    sd x3,  (2 * 8)(sp)
    sd x4,  (3 * 8)(sp)
    sd x5,  (4 * 8)(sp)
    sd x6,  (5 * 8)(sp)
    sd x7,  (6 * 8)(sp)
    sd x8,  (7 * 8)(sp)
    sd x9,  (8 * 8)(sp)
    sd x10, (9 * 8)(sp)
    sd x11, (10 * 8)(sp)
    sd x12, (11 * 8)(sp)
    sd x13, (12 * 8)(sp)
    sd x14, (13 * 8)(sp)
    sd x15, (14 * 8)(sp)
    sd x16, (15 * 8)(sp)
    sd x17, (16 * 8)(sp)
    sd x18, (17 * 8)(sp)
    sd x19, (18 * 8)(sp)
    sd x20, (19 * 8)(sp)
    sd x21, (20 * 8)(sp)
    sd x22, (21 * 8)(sp)
    sd x23, (22 * 8)(sp)
    sd x24, (23 * 8)(sp)
    sd x25, (24 * 8)(sp)
    sd x26, (25 * 8)(sp)
    sd x27, (26 * 8)(sp)
    sd x28, (27 * 8)(sp)
    sd x29, (28 * 8)(sp)
    sd x30, (29 * 8)(sp)
    sd x31, (30 * 8)(sp)

    # Save Machine-level CSRs onto the stack.
    csrr t0, mepc
    sd t0, (31 * 8)(sp) # Save mepc
    csrr t0, mcause
    sd t0, (32 * 8)(sp) # Save mcause
    csrr t0, mtval
    sd t0, (33 * 8)(sp) # Save mtval

    # Call the C trap handler.
    mv a0, sp
    call mtrap_handler

    # Restore Machine-level CSRs from the stack.
    ld t0, (33 * 8)(sp) # Load mtval
    csrw mtval, t0
    ld t0, (32 * 8)(sp) # Load mcause
    csrw mcause, t0
    ld t0, (31 * 8)(sp) # Load mepc
    csrw mepc, t0

    # Restore general-purpose registers (x1-x31) from the stack.
    ld x1,  (0 * 8)(sp)
    ld x2,  (1 * 8)(sp)
    ld x3,  (2 * 8)(sp)
    ld x4,  (3 * 8)(sp)
    ld x5,  (4 * 8)(sp)
    ld x6,  (5 * 8)(sp)
    ld x7,  (6 * 8)(sp)
    ld x8,  (7 * 8)(sp)
    ld x9,  (8 * 8)(sp)
    ld x10, (9 * 8)(sp)
    ld x11, (10 * 8)(sp)
    ld x12, (11 * 8)(sp)
    ld x13, (12 * 8)(sp)
    ld x14, (13 * 8)(sp)
    ld x15, (14 * 8)(sp)
    ld x16, (15 * 8)(sp)
    ld x17, (16 * 8)(sp)
    ld x18, (17 * 8)(sp)
    ld x19, (18 * 8)(sp)
    ld x20, (19 * 8)(sp)
    ld x21, (20 * 8)(sp)
    ld x22, (21 * 8)(sp)
    ld x23, (22 * 8)(sp)
    ld x24, (23 * 8)(sp)
    ld x25, (24 * 8)(sp)
    ld x26, (25 * 8)(sp)
    ld x27, (26 * 8)(sp)
    ld x28, (27 * 8)(sp)
    ld x29, (28 * 8)(sp)
    ld x30, (29 * 8)(sp)
    ld x31, (30 * 8)(sp)

    # Deallocate space for the MTrapFrame from the stack.
    addi sp, sp, 272

    # Return from trap using mret (Machine Return).
    mret
