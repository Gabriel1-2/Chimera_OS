#include "vm.h"
#include "mem.h"
#include <string.h>

// Sv39 uses three levels:
//   Level 2: Bits 38-30, Level 1: Bits 29-21, Level 0: Bits 20-12.
#define VPN_MASK 0x1FFUL    // 9-bit mask.
#define VPN_SHIFT_LVL2 30
#define VPN_SHIFT_LVL1 21
#define VPN_SHIFT_LVL0 12

/*
 * Create a new page table.
 * Allocates one page using pfa_alloc and zeros it.
 */
pagetable_t vm_create_pagetable(void) {
    pagetable_t pt = (pagetable_t) pfa_alloc();
    if (pt) {
        memset(pt, 0, PAGE_SIZE);
    }
    return pt;
}

/*
 * Map virtual address 'va' to physical address 'pa' with given flags.
 * Walks through Level 2, Level 1, and Level 0 page tables.
 */
void vm_map(pagetable_t root, uint64_t va, uint64_t pa, uint64_t flags) {
    pagetable_t level = root;
    uint64_t vpn2 = (va >> VPN_SHIFT_LVL2) & VPN_MASK;
    uint64_t vpn1 = (va >> VPN_SHIFT_LVL1) & VPN_MASK;
    uint64_t vpn0 = (va >> VPN_SHIFT_LVL0) & VPN_MASK;

    // Level 2: Create page if not exists.
    if (!(level[vpn2] & PTE_V)) {
        pagetable_t new_level = vm_create_pagetable();
        level[vpn2] = (((uint64_t)new_level) >> 12 << 10) | PTE_V;
    }
    level = (pagetable_t)(((uint64_t)level[vpn2] >> 10) << 12);

    // Level 1: Create page if not exists.
    if (!(level[vpn1] & PTE_V)) {
        pagetable_t new_level = vm_create_pagetable();
        level[vpn1] = (((uint64_t)new_level) >> 12 << 10) | PTE_V;
    }
    level = (pagetable_t)(((uint64_t)level[vpn1] >> 10) << 12);

    // Level 0: Set the final mapping.
    level[vpn0] = ((pa >> 12) << 10) | (flags | PTE_V);
}
