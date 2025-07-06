#include "mem.h"
#include "uart.h"
#include "panic.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// --- Memory Layout Definitions ---
#define MEM_START_ADDR      0x80000000UL          // Base of physical memory.
#define MEM_SIZE_BYTES      (128UL * 1024 * 1024) // 128MB total.
#define PAGE_SIZE_BYTES     4096UL                // 4KB per page.

#define NUM_PAGES           (MEM_SIZE_BYTES / PAGE_SIZE_BYTES)
#define BITMAP_SIZE_BYTES   ((NUM_PAGES + 7) / 8)  // Size of bitmap in bytes.

// --- Kernel and Bitmap Occupancy ---
// Assume kernel occupies the first 1MB (256 pages).
#define KERNEL_SIZE_PAGES   (1UL * 1024 * 1024 / PAGE_SIZE_BYTES)

// --- PFA Internal State ---
static uint8_t pfa_bitmap[BITMAP_SIZE_BYTES];

static int get_bit(uint64_t page_idx) {
    if (page_idx >= NUM_PAGES) {
        uart_puts("Error: get_bit() for invalid page index.\n");
        return -1;
    }
    return (pfa_bitmap[page_idx / 8] >> (page_idx % 8)) & 1;
}

static void set_bit(uint64_t page_idx) {
    if (page_idx >= NUM_PAGES) {
        uart_puts("Error: set_bit() for invalid page index.\n");
        return;
    }
    pfa_bitmap[page_idx / 8] |= (1 << (page_idx % 8));
}

static void clear_bit(uint64_t page_idx) {
    if (page_idx >= NUM_PAGES) {
        uart_puts("Error: clear_bit() for invalid page index.\n");
        return;
    }
    pfa_bitmap[page_idx / 8] &= ~(1 << (page_idx % 8));
}

void pfa_init(void) {
    memset(pfa_bitmap, 0, BITMAP_SIZE_BYTES);

    // Mark pages occupied by the kernel.
    for (uint64_t i = 0; i < KERNEL_SIZE_PAGES; i++) {
        set_bit(i);
    }
    // Mark pages occupied by the bitmap itself.
    uint64_t bitmap_start_byte_offset = KERNEL_SIZE_PAGES * PAGE_SIZE_BYTES;
    uint64_t bitmap_start_page_idx = bitmap_start_byte_offset / PAGE_SIZE_BYTES;
    uint64_t bitmap_size_pages = (BITMAP_SIZE_BYTES + PAGE_SIZE_BYTES - 1) / PAGE_SIZE_BYTES;
    for (uint64_t i = 0; i < bitmap_size_pages; i++) {
        set_bit(bitmap_start_page_idx + i);
    }
    uart_puts("Memory allocator (PFA) initialized.\n");
}

void* pfa_alloc(void) {
    for (uint64_t i = 0; i < NUM_PAGES; i++) {
        if (get_bit(i) == 0) {
            set_bit(i);
            void* addr = (void*)(MEM_START_ADDR + (i * PAGE_SIZE_BYTES));
            if (((uint64_t)addr % PAGE_SIZE_BYTES) != 0) {
                uart_puts("Critical: Page allocation returned misaligned address!\n");
                panic("pfa_alloc alignment error");
            }
            return addr;
        }
    }
    uart_puts("Warning: pfa_alloc found no free pages!\n");
    return NULL;
}

void pfa_free(void* ptr) {
    if (ptr == NULL) {
        uart_puts("Warning: pfa_free called with NULL pointer.\n");
        return;
    }
    uint64_t addr = (uint64_t)ptr;
    if (addr < MEM_START_ADDR || addr >= (MEM_START_ADDR + MEM_SIZE_BYTES)) {
        uart_puts("Error: Attempt to free memory outside managed region.\n");
        panic("pfa_free region error");
    }
    if (addr % PAGE_SIZE_BYTES != 0) {
        uart_puts("Error: Attempt to free non-page-aligned memory.\n");
        panic("pfa_free alignment error");
    }
    uint64_t page_idx = (addr - MEM_START_ADDR) / PAGE_SIZE_BYTES;
    clear_bit(page_idx);
}
