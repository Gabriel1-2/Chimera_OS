#
# Project Chimera - Boot Assembly
#
# Target: RISC-V 64-bit (rv64g)
#
# This is the first code to execute on the CPU. Its primary job is to perform
# the initial hardware setup before handing control over to the C runtime.
#

.section .text

.global _start
.global kmain
.global __trap_vector

_start:
  # Set up a basic stack pointer.
  # We'll use a fixed address for now, ensuring it's above our code.
  li sp, 0x80200000

  # Set the Machine Trap-Vector Base-Address Register (mtvec)
  # to point to our trap handler.
  la t0, __trap_vector
  csrw mtvec, t0

  # Jump to the C kernel's main entry point.
  j kmain

  # In case kmain ever returns (which it shouldn't in a bare-metal OS),
  # we'll just halt the CPU to prevent unpredictable behavior.
halt:
  j halt