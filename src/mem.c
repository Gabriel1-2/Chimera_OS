#include "mem.h"
#include <stddef.h> // For NULL

// --- Memory Layout Definitions ---
#define MEM_START_ADDR      0x80000000UL  // Physical memory starts at 0x80000000
#define MEM_SIZE_BYTES      (128UL * 1024 * 1024) // 128 MB of physical memory
#define PAGE_SIZE_BYTES     4096UL        // 4 KB page size

// --- Calculated Values ---
#define NUM_PAGES           (MEM_SIZE_BYTES / PAGE_SIZE_BYTES)
#define BITMAP_SIZE_BYTES   ((NUM_PAGES + 7) / 8) // Size of bitmap in bytes

// --- Kernel and Bitmap Occupancy (Hardcoded for now) ---
// Assuming kernel occupies the first 1MB (256 pages) for simplicity
#define KERNEL_SIZE_PAGES   (1UL * 1024 * 1024 / PAGE_SIZE_BYTES) // 1MB / 4KB = 256 pages

// --- PFA Internal State ---
// The bitmap itself. It must be placed in a known memory location
// that is not managed by the PFA initially.
// For simplicity, we place it right after the assumed kernel space.
// In a real OS, this would be carefully managed.
static uint8_t pfa_bitmap[BITMAP_SIZE_BYTES];

// --- Helper Functions (Internal) ---

// Get the bit status for a given page index
static int get_bit(uint64_t page_idx) {
    if (page_idx >= NUM_PAGES) return -1; // Invalid page index
    return (pfa_bitmap[page_idx / 8] >> (page_idx % 8)) & 1;
}

// Set the bit status for a given page index
static void set_bit(uint64_t page_idx) {
    if (page_idx >= NUM_PAGES) return; // Invalid page index
    pfa_bitmap[page_idx / 8] |= (1 << (page_idx % 8));
}

// Clear the bit status for a given page index
static void clear_bit(uint64_t page_idx) {
    if (page_idx >= NUM_PAGES) return; // Invalid page index
    pfa_bitmap[page_idx / 8] &= ~(1 << (page_idx % 8));
}

// --- PFA Public API Implementations ---

void pfa_init(void) {
    // 1. Mark all pages as free initially
    for (uint64_t i = 0; i < BITMAP_SIZE_BYTES; i++) {
        pfa_bitmap[i] = 0; // All bits to 0 (free)
    }

    // 2. Calculate and mark pages occupied by the kernel and the bitmap itself

    // Pages occupied by the kernel
    for (uint64_t i = 0; i < KERNEL_SIZE_PAGES; i++) {
        set_bit(i);
    }

    // Pages occupied by the PFA bitmap
    // The bitmap starts at MEM_START_ADDR + (KERNEL_SIZE_PAGES * PAGE_SIZE_BYTES)
    // Calculate its start page index
    uint64_t bitmap_start_byte_offset = KERNEL_SIZE_PAGES * PAGE_SIZE_BYTES;
    uint64_t bitmap_start_page_idx = bitmap_start_byte_offset / PAGE_SIZE_BYTES;

    // Calculate how many pages the bitmap occupies
    uint64_t bitmap_size_pages = (BITMAP_SIZE_BYTES + PAGE_SIZE_BYTES - 1) / PAGE_SIZE_BYTES;

    for (uint64_t i = 0; i < bitmap_size_pages; i++) {
        set_bit(bitmap_start_page_idx + i);
    }
}

void* pfa_alloc(void) {
    // Search for the first free page (a '0' bit)
    for (uint64_t i = 0; i < NUM_PAGES; i++) {
        if (get_bit(i) == 0) {
            // Found a free page
            set_bit(i); // Mark it as used
            // Return the physical address of this page
            return (void*)(MEM_START_ADDR + (i * PAGE_SIZE_BYTES));
        }
    }
    return NULL; // No free page found
}

void pfa_free(void* ptr) {
    if (ptr == NULL) return; // Cannot free NULL pointer

    uint64_t addr = (uint64_t)ptr;

    // Ensure the address is within the managed memory region
    if (addr < MEM_START_ADDR || addr >= (MEM_START_ADDR + MEM_SIZE_BYTES)) {
        // Address is outside managed memory, or not page-aligned
        return;
    }

    // Calculate which page this address belongs to
    uint64_t page_idx = (addr - MEM_START_ADDR) / PAGE_SIZE_BYTES;

    // Clear the corresponding bit in the bitmap, marking it as free
    clear_bit(page_idx);
}
