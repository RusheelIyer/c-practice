#ifndef PAGE_FAULTS_H
#define PAGE_FAULTS_H

#include <inttypes.h>

// You can use these masks if you want to.
#define BITS_PER_ENTRY 10
#define ENTRIES_PER_TABLE (1 << BITS_PER_ENTRY)

#define OFFSET_BITS 12
#define OFFSET_MASK ((1UL << OFFSET_BITS) - 1)
#define BASE_MASK (~OFFSET_MASK)

// The bits of the entry for the position of the page table.
#define PAGE_DIRECTORY_ADDRESS_MASK (~((uint64_t)0xfff))

// The bits of the entry for the position of the page.
#define PAGE_TABLE_ADDRESS_MASK (~((uint32_t)0xfff))

#define PAGE_ACCESSED_MASK 0x20

// Allow user mode access
#define PAGE_USERMODE_MASK 0x04

// Allow write access
#define PAGE_READWRITE_MASK 0x02

// This page is currently swapped to disk
#define PAGE_SWAPPED_MASK 0x200

// The present bit.
#define PAGE_PRESENT_MASK 0x01

typedef enum {
    ACCESS_READ, ACCESS_WRITE
} ReadWrite;

typedef enum {
    USER_MODE, KERNEL_MODE
} PrivilegeLevel;

// The first-level page table - the page directory.
// The table must be 4KiB aligned. The size of this table is 8KiB. We use 64
// bits per entry to be able to test this on 64 bit systems. On 32 bit systems,
// 32 bits would be sufficient.
typedef struct {
    // Bits used in our implementation:
    // Bits 12 - 63: The address of the page table.
    // Bit 0: This page table is present.
    // All other bits reserved.
    uint64_t entries[ENTRIES_PER_TABLE];
} PageDirectory;

// Bits that should be preserved on a swap out operation
#define PRESERVED_BITS_ON_SWAP \
    (PAGE_ACCESSED_MASK | PAGE_USERMODE_MASK | PAGE_READWRITE_MASK)

// The second-level page table.
// The table must be 4KiB aligned.
typedef struct {
    // Bits used in our implementation:
    // Bits 12 - 31: The physical frame to which this page is mapped.
    // Bit 9: This page has been swapped out
    // Bit 6: The accessed bit.
    // Bit 2: This page is accessible in user mode.
    // Bit 1: This page is writable.
    // Bit 0: This page is present.
    uint32_t entries[ENTRIES_PER_TABLE];
} PageTable;

// Converts a pointer to an integer without compiler warnings.
static inline uint64_t pointerToInt(void* ptr)
{
    return (uint64_t)((intptr_t)ptr);
}

// Converts an integer to a pointer without compiler warnings.
static inline void* intToPointer(uint64_t ptr)
{
    return (void*)((intptr_t)ptr);
}

void setPageDirectory(PageDirectory *directory);

int mapPage(uint32_t virtualBase, uint32_t physicalBase, ReadWrite accessMode,
        PrivilegeLevel privileges);

int translatePageTable(uint32_t *address, ReadWrite accessMode,
        PrivilegeLevel privileges);

int handlePageFault(uint32_t virtualBase, uint32_t pte);

int swapOut(uint32_t virtualBase);

/* Necessary for test_swapOut_disk_offset */
__attribute__((weak))
uint32_t _storeOnDisk(uint32_t physAddress);
/* test_handlePageFault_load */
__attribute__((weak))
uint32_t _loadFromDisk(uint32_t diskAddress);

#endif
