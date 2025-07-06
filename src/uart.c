#include "uart.h"

// UART base address and register offsets
#define UART_BASE 0x10000000
#define UART_THR 0  // Transmitter Holding Register
#define UART_LSR 5  // Line Status Register

// Line Status Register bits
#define LSR_THRE (1 << 5) // Transmitter Holding Register Empty

void uart_putc(char c) {
    volatile unsigned char *uart = (volatile unsigned char *)UART_BASE;

    // Wait until the Transmitter Holding Register is empty
    while (! (uart[UART_LSR] & LSR_THRE));

    // Write the character to the Transmitter Holding Register
    uart[UART_THR] = c;
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

void uart_puts_hex(uint64_t n) {
    uart_putc('0');
    uart_putc('x');

    // Iterate through 16 nibbles (64 bits / 4 bits per nibble)
    for (int i = 0; i < 16; i++) {
        // Extract the current nibble (from MSB to LSB)
        // Shift right by (15 - i) * 4 to get the current nibble to the LSB position
        // Then mask with 0xF to get only the 4 bits of the nibble
        uint8_t nibble = (n >> ((15 - i) * 4)) & 0xF;

        // Convert nibble to ASCII character
        if (nibble < 10) {
            uart_putc('0' + nibble);
        } else {
            uart_putc('a' + (nibble - 10));
        }
    }
}