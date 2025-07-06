#include "uart.h"

/*
 * This is our first user-space process.
 * It enters an infinite loop that prints an identifying message.
 * It never returns.
 */
void user_process_1(void) {
    while (1) {
        uart_puts("... I am user process 1 ...\n");
        // Optionally add some delay (e.g., busy loop) here.
        for(volatile uint64_t i = 0; i < 1000000; i++);
    }
}
