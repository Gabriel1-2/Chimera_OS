#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_puts(const char *s);
void uart_puts_hex(uint64_t n);

#endif // UART_H