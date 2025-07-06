#ifndef MEM_H
#define MEM_H

#include <stdint.h>

// Initialize the Physical Page Frame Allocator
void pfa_init(void);

// Allocate a single physical page frame
void* pfa_alloc(void);

// Free a previously allocated physical page frame
void pfa_free(void* ptr);

#endif // MEM_H
