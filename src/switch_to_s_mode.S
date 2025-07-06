#
# Project Chimera - Machine to Supervisor Mode Switch Assembly
#
# This file provides an assembly function to perform the transition
# from Machine mode to Supervisor mode.
#

.section .text
.global switch_to_s_mode

# switch_to_s_mode(hartid, dtb_paddr, supervisor_entry_point)
# a0: hartid
# a1: dtb_paddr
# a2: supervisor_entry_point (address of _start_supervisor_asm)
switch_to_s_mode:
    # Save a0 and a1 (hartid and dtb_paddr) to s0 and s1
    # These will be passed to _start_supervisor_c
    mv s0, a0
    mv s1, a1

    # Set MEPC to the supervisor entry point (a2)
    csrw mepc, a2

    # Configure MSTATUS for S-mode transition
    # Read current mstatus
    csrr t0, mstatus

    # Clear MPP (Machine Previous Privilege) field (bits 11-12) to 00 (User mode)
    # Then set MPP to 01 (Supervisor mode)
    li t1, ~(3 << 11) # Mask to clear MPP bits
    and t0, t0, t1
    li t1, (1 << 11)  # Value for Supervisor mode (01)
    or t0, t0, t1

    # Set MPIE (Machine Previous Interrupt Enable) to 1 (enable interrupts on return)
    li t1, (1 << 7)
    or t0, t0, t1

    # Write updated mstatus back
    csrw mstatus, t0

    # Execute mret to transition to Supervisor mode and jump to supervisor_entry_point
    mret
    wfi
