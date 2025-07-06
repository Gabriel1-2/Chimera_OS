#include "panic.h"
#include "uart.h"

void panic(const char* msg) {
    uart_puts("Kernel Panic: ");
    uart_puts(msg);
    uart_puts("\nHalting system.\n");
    while (1) {}
}
