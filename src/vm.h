#ifndef VM_H
#define VM_H

#include <stdint.h>
#include "mem.h"

#define PAGE_SIZE 4096

// Sv39 Page Table Entry flags.
#define PTE_V (1UL << 0) // Valid
#define PTE_R (1UL << 1) // Read
#define PTE_W (1UL << 2) // Write
#define PTE_X (1UL << 3) // Execute

typedef uint64_t* pagetable_t;

/*
 * Create a new pagetable by allocating a zeroed page.
 */
pagetable_t vm_create_pagetable(void);

/*
 * Map a virtual address 'va' to a physical address 'pa' with provided flags in the page table 'root'.
 * Implements a 3-level Sv39 walk.
 */
void vm_map(pagetable_t root, uint64_t va, uint64_t pa, uint64_t flags);

#endif // VM_H
