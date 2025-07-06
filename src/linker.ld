/*
 * Chimera OS Linker Script for Sv39-based Virtual Memory.
 * Memory starts at 0x80000000.
 */
OUTPUT_ARCH(riscv)
ENTRY(_start)
SECTIONS
{
    . = 0x80000000;

    _text_start = .;
    .text : {
        *(.text.init)
        *(.text .text.*)
    }
    _text_end = .;

    _rodata_start = .;
    .rodata : {
        *(.rodata .rodata.*)
    }
    _rodata_end = .;

    .data : {
        *(.data .data.*)
    }
    .bss : {
        *(.bss .bss.*)
        _memory_end = .;
    }
}
