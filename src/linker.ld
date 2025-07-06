/*
 * Chimera OS - Linker Script
 *
 * Target: RISC-V 64-bit (QEMU 'virt' machine)
 *
 * This script defines the memory layout for a bare-metal program
 * running on the QEMU 'virt' machine, which starts execution
 * at the beginning of DRAM (0x80000000).
 */

/*
 * Specify the output architecture.
 * 'riscv' will be automatically translated to the correct format,
 * e.g., 'elf64-littleriscv'.
 */
OUTPUT_ARCH(riscv)

/*
 * Define the entry point of the program. This is the first
 * symbol the CPU will execute. We name it '_start'.
 */
ENTRY(_start)

/*
 * Define the sections of the final executable.
 */
SECTIONS
{
  /*
   * Set the origin point. The '.' symbol is the location counter,
   * and we set it to the start of the 'virt' machine's RAM.
   */
  . = 0x80000000;

  /*
   * The .text section contains all the executable code.
   * We place all input .text sections from the object files here.
   * 'KEEP' ensures the linker doesn't discard the .text.init section.
   */
  .text : {
    *(.text.init)
    *(.text .text.*)
  }

  /*
   * The .rodata section contains read-only data, such as string literals.
   */
  .rodata : {
    *(.rodata .rodata.*)
  }

  /*
   * The .data section contains initialized global and static variables.
   */
  .data : {
    *(.data .data.*)
  }

  /*
   * The .bss section contains uninitialized global and static variables.
   * These are zero-initialized by the C runtime (though we don't have one yet).
   */
  .bss : {
    *(.bss .bss.*)
  }

  /*
   * Define the end of the BSS section, which can be useful for memory management.
   */
  _end = .;
}