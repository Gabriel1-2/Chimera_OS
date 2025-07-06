#
# Project Chimera - Supervisor Mode Entry Assembly
#
# This file provides a simple assembly entry point for the kernel
# when transitioning from Machine mode to Supervisor mode.
#

.section .text
.global _start_supervisor_asm

_start_supervisor_asm:
    # Jump to the C entry point for Supervisor mode.
    # The arguments (hartid, dtb_paddr) are already in a0 and a1
    # due to the RISC-V calling convention and how kmain was called.
    j _start_supervisor_c
