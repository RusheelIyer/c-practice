#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <inttypes.h>

#define TLB_SIZE 4

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

// Allow usermode access
#define PAGE_USERMODE_MASK 0x04

// Allow write access
#define PAGE_READWRITE_MASK 0x02

// The present bit.
#define PAGE_PRESENT_MASK 0x01

typedef enum {
    ACCESS_READ, ACCESS_WRITE
} ReadWrite;

typedef enum {
    USER_MODE, KERNEL_MODE
} PrivilegeLevel;

// The 4kiB aligned page directory.
// The size of this table is 8kiB. We use 64 bits per entry to be
// able to test this on 64 bit systems. On 32 bit systems, 32 bits would
// be sufficient.
typedef struct {
    // Bits used in our implementation:
    // Bits 12 - 63: The address of the page table.
    // Bit 0: The present bit.
    // All other bits reserved.
    uint64_t entries[ENTRIES_PER_TABLE];
} PageDirectory;

// The 4kiB aligned page table.
typedef struct {
    // Bits used in our implementation:
    // Bits 12 - 31: Physical base address of the target frame.
    // Bit 6: The accessed bit.
    // Bit 2: This page is accessible in user mode.
    // Bit 1: This page is writable.
    // Bit 0: The present bit.
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

void flushTLB();

int addToTLB(uint32_t virtualBase, uint32_t pageTableEntry);

void invalidateTLBEntry(uint32_t virtualBase);

int translateTLB(uint32_t *address, ReadWrite accessMode,
        PrivilegeLevel privileges);

#endif

