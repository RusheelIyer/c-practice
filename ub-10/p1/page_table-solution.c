#define _POSIX_C_SOURCE 200112L
#include "page_table.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

// The pointer to the base directory.
// You can safely assume that this is set before any address conversion is done.
static PageDirectory *_cr3 = NULL;

typedef struct {
    uint32_t accessCounter;
    int valid;

    uint32_t virtualBase; // This is the TLB tag: the virtual base address.
    uint32_t pte;         // The page table entry. The accessed bit is ignored.
} TLBEntry;

typedef struct {
    TLBEntry entries[TLB_SIZE];
    uint32_t accessCounter;
} TLB;

// This is reset to all 0.
static TLB _tlb;

void setPageDirectory(PageDirectory *directory)
{
    _cr3 = directory;
    flushTLB();
}

void flushTLB()
{
    memset(&_tlb, 0, sizeof(_tlb));
}

#define ENTRY_MASK (ENTRIES_PER_TABLE - 1)

// Returns the base address of the current frame
// (i.e., the address of the first byte in the frame)
static inline uint32_t _getVirtualBase(uint32_t address)
{
    return address & BASE_MASK;
}

// Returns the index in the page directory for the given address.
static inline uint32_t _getPageDirectoryIndex(uint32_t address)
{
    return address >> (OFFSET_BITS + BITS_PER_ENTRY);
}

// Returns the index in the second level page table for the given address.
static inline uint32_t _getPageTableIndex(uint32_t address)
{
    return (address >> OFFSET_BITS) & ENTRY_MASK;
}

// Returns the offset for the given address.
static inline uint32_t _getOffset(uint32_t address)
{
    return address & OFFSET_MASK;
}

// Sets the page table entry, allocates a new table if required.
static int _setPte(uint32_t virtualBase, uint32_t pte)
{
    assert(_cr3 != NULL);
    assert(_getOffset(virtualBase) == 0);

    // (1) Get the page table from the given address
    uint32_t pdi = _getPageDirectoryIndex(virtualBase);
    assert(pdi < ENTRIES_PER_TABLE);

    uint64_t pde = _cr3->entries[pdi];
    PageTable *pageTable = NULL;
    if (!(pde & PAGE_PRESENT_MASK)) {
        // The page table has not been allocated yet. Allocate a new,
        // aligned one and clear it to reset all present bits in the PTEs.
        if (posix_memalign((void**)&pageTable, sizeof(PageTable),
                sizeof(PageTable)) != 0) {
            return -1;
        }

        assert(pageTable != NULL);
        memset(pageTable, 0, sizeof(PageTable));

        // Register the new page table in the page directory.
        uint64_t address = pointerToInt(pageTable);
        assert((address & OFFSET_MASK) == 0);

        pde = address | PAGE_PRESENT_MASK;
        _cr3->entries[pdi] = pde;
    } else {
        uint64_t pageTableAddress = pde & PAGE_DIRECTORY_ADDRESS_MASK;
        pageTable = (PageTable*)intToPointer(pageTableAddress);
    }

    // (2) Set the specified page table entry
    uint32_t pti = _getPageTableIndex(virtualBase);
    assert(pti < ENTRIES_PER_TABLE);

    pageTable->entries[pti] = pte;

    return 0;
}

// Gets the entry in the page table or 0 if there is no entry yet.
static uint32_t _getPte(uint32_t virtualBase)
{
    assert(_cr3 != NULL);
    assert(_getOffset(virtualBase) == 0);

    // (1) Get the page table from the given address
    uint32_t pdi = _getPageDirectoryIndex(virtualBase);
    assert(pdi < ENTRIES_PER_TABLE);

    uint64_t pde = _cr3->entries[pdi];
    if (!(pde & PAGE_PRESENT_MASK)) {
        // The page table is not allocated. We return 0 as an PTE, which will
        // have the present bit cleared.
        return 0;
    }

    uint64_t pageTableAddress = pde & PAGE_DIRECTORY_ADDRESS_MASK;
    PageTable *pageTable = (PageTable*)intToPointer(pageTableAddress);

    // (2) Get and return the PTE
    uint32_t pti = _getPageTableIndex(virtualBase);
    assert(pti < ENTRIES_PER_TABLE);

    return pageTable->entries[pti];
}

int mapPage(uint32_t virtualBase, uint32_t physicalBase, ReadWrite accessMode,
    PrivilegeLevel privileges)
{
    if ((_getOffset(virtualBase) != 0) || (_getOffset(physicalBase) != 0)) {
        return -1;
    }

    // Build the page table entry from the physical base and flags
    uint32_t pte = physicalBase | PAGE_PRESENT_MASK;

    if (accessMode == ACCESS_WRITE) {
        pte |= PAGE_READWRITE_MASK;
    }

    if (privileges == USER_MODE) {
        pte |= PAGE_USERMODE_MASK;
    }

    // Set the new PTE and invalidate any cached version in the TLB
    int res = _setPte(virtualBase, pte);
    invalidateTLBEntry(virtualBase);

    return res;
}

static int _translateByEntry(uint32_t *address, ReadWrite accessMode,
    PrivilegeLevel privileges, uint32_t pte)
{
    assert(address != NULL);
    assert(pte & PAGE_PRESENT_MASK);

    // If we are in user mode, we have to perform permission checks.
    if (privileges == USER_MODE) {
        if (!(pte & PAGE_USERMODE_MASK)) {
            return -1; // User mode attempted to access a kernel page.
        }

        if ((accessMode == ACCESS_WRITE) && !(pte & PAGE_READWRITE_MASK)) {
            return -1; // User mode attempted to write to a read-only page.
        }
    }

    *address = (pte & PAGE_TABLE_ADDRESS_MASK) + _getOffset(*address);
    return 0;
}

int translatePageTable(uint32_t *address, ReadWrite accessMode,
    PrivilegeLevel privileges)
{
    if (address == NULL) {
        return -1;
    }

    uint32_t vab = _getVirtualBase(*address);
    uint32_t pte = _getPte(vab);

    if (!(pte & PAGE_PRESENT_MASK)) {
        return -1; // Page Fault. Reason: Not present
    }

    // TLB add policy: Only add to TLB if page access is allowed.
    if (_translateByEntry(address, accessMode, privileges, pte) == 0) {
        int r = addToTLB(vab, pte);
        assert(r == 0);

        pte |= PAGE_ACCESSED_MASK;
        return _setPte(vab, pte);
    } else {
        return -1; // Page Fault. Reason: Permission violation
    }
}

void invalidateTLBEntry(uint32_t virtualBase)
{
    assert(_getOffset(virtualBase) == 0);

    for (int i = 0; i < TLB_SIZE; i++) {
        if (_tlb.entries[i].virtualBase == virtualBase) {
            _tlb.entries[i].valid = 0;
        }
    }
}

static void _addToTLBAt(int index, uint32_t virtualBase, uint32_t pte)
{
    _tlb.entries[index].accessCounter = _tlb.accessCounter;
    _tlb.entries[index].valid = 1;
    _tlb.accessCounter++;

    _tlb.entries[index].virtualBase = virtualBase;
    _tlb.entries[index].pte = pte;
}

int addToTLB(uint32_t virtualBase, uint32_t pte)
{
    if (_getOffset(virtualBase) != 0) {
        return -1;
    }

    for (int i = 0; i < TLB_SIZE; i++) {
        assert((!_tlb.entries[i].valid) ||
               (virtualBase != _tlb.entries[i].virtualBase));
    }

    int oldestEntry = 0;
    for (int i = 1; i < TLB_SIZE; i++) {
        if (!_tlb.entries[i].valid) {
            oldestEntry = i;
            break;
        }

        if (_tlb.entries[i].accessCounter < _tlb.entries[oldestEntry].accessCounter) {
            oldestEntry = i;
        }
    }

    _addToTLBAt(oldestEntry, virtualBase, pte);

    return 0;
}

int translateTLB(uint32_t *address, ReadWrite accessMode,
    PrivilegeLevel privileges)
{
    if (address == NULL) {
        return -1;
    }

    uint32_t vab = _getVirtualBase(*address);
    for (int i = 0; i < TLB_SIZE; i++) {
        if (_tlb.entries[i].valid && (_tlb.entries[i].virtualBase == vab)) {
            uint32_t pte = _tlb.entries[i].pte;
            return _translateByEntry(address, accessMode, privileges, pte);
        }
    }

    return -1;
}