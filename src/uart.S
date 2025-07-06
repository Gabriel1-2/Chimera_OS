#
# Project Chimera - NS16550a UART Driver
#
# Target: RISC-V 64-bit (rv64g)
#
# This file provides basic functions for serial output using the UART
# on QEMU's 'virt' machine.
#

# --- UART Configuration --
# The base memory-mapped address of the UART controller.
.equ UART_BASE, 0x10000000

# UART register offsets.
.equ UART_THR, 0  # Transmitter Holding Register (write-only)
.equ UART_LSR, 5  # Line Status Register (read-only)

# Line Status Register bits.
.equ LSR_THRE, (1 << 5) # Transmitter Holding Register Empty

# --- Code Section ---
.section .text

# --- Global Symbols ---
# We make our functions globally visible so they can be called from other files.
.global uart_putc
.global uart_puts

# --- uart_putc Function ---
#
# Prints a single character to the UART.
#
# @param a0: The 8-bit character to print.
# @clobbers t0, t1: Uses temporary registers for operation.
#
uart_putc:
  # Load the UART base address into a temporary register.
  li t0, UART_BASE

.Lputc_wait:
  # Load the Line Status Register (LSR) value.
  # LSR is at an offset of +5 bytes from the base address.
  lbu t1, UART_LSR(t0)

  # Check if the Transmitter Holding Register Empty (THRE) bit is set.
  # We isolate the bit by ANDing with our mask.
  andi t1, t1, LSR_THRE

  # If the result is zero, the transmitter is not ready. Loop back.
  beqz t1, .Lputc_wait

  # The transmitter is ready. Write the character from a0 to the
  # Transmitter Holding Register (THR) at offset +0.
  sb a0, UART_THR(t0)

  # Return to the caller.
  ret

# --- uart_puts Function ---
#
# Prints a null-terminated string to the UART.
#
# @param a0: The memory address of the first character in the string.
#
uart_puts:
  # --- Prologue: Save Return Address ---
  # The `call` instruction places the return address in `ra`. Since we are
  # making our own calls to `uart_putc`, we must save `ra` to the stack to
  # prevent it from being overwritten.
  addi sp, sp, -8  # Allocate 8 bytes on the stack
  sd ra, 0(sp)     # Save ra to the stack

  # Move the string address from a0 to a saved register (s1) so we can
  # use a0 to pass arguments to uart_putc.
  mv s1, a0

.Lputs_loop:
  # Load the next byte (character) from the string address in s1.
  lbu a0, 0(s1)

  # Check if the loaded byte is the null terminator.
  # If it is, we are done with the string.
  beqz a0, .Lputs_done

  # If not null, it's a valid character. Call uart_putc to print it.
  # The character to print is already in a0, as required.
  call uart_putc

  # Increment the string pointer to the next character.
  addi s1, s1, 1

  # Jump back to the start of the loop.
  j .Lputs_loop

.Lputs_done:
  # --- Epilogue: Restore Return Address ---
  # Restore the original return address from the stack.
  ld ra, 0(sp)     # Load ra from the stack
  addi sp, sp, 8   # Deallocate space from the stack

  # Return to the original caller.
  ret
