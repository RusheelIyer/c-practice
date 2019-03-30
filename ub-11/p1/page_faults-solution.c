#define _POSIX_C_SOURCE 200112L
#include "page_faults.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

typedef enum {
    // A zeroed memory page.
    VMA_TYPE_NULL,
    // A memory mapped file.
    VMA_TYPE_FILE
} VMAType;

typedef struct {
    VMAType type;

    uint32_t virtualStart;
    uint32_t length;

    ReadWrite accessMode;
    PrivilegeLevel privileges;

    // The file name (only for type == VMA_TYPE_FILE)
    const char* fileName;
} VMA;

static VMA _vmas[] = {
    { .type = VMA_TYPE_NULL,
      .virtualStart = 0x10000,
      .length = 0x8000,
      .accessMode = ACCESS_WRITE,
      .privileges = USER_MODE },
    { .type = VMA_TYPE_FILE,
      .virtualStart = 0x30000,
      .length = 0x8000,
      .accessMode = ACCESS_READ,
      .privileges = USER_MODE,
      .fileName = "/tmp/tmpfile" },
    { .type = VMA_TYPE_FILE,
      .virtualStart = 0x8563000,
      .length = 0x3000,
      .accessMode = ACCESS_WRITE,
      .privileges = KERNEL_MODE,
      .fileName = "/kernel" },
};

// The pointer to the base directory.
static PageDirectory *_cr3 = NULL;

void setPageDirectory(PageDirectory *directory)
{
    _cr3 = directory;
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
    return _setPte(virtualBase, pte);
}

static int _translateByEntry(uint32_t *address, ReadWrite accessMode,
    PrivilegeLevel privileges, uint32_t pte)
{
    assert(address != NULL);
    assert(pte & PAGE_PRESENT_MASK);
    assert(!(pte & PAGE_SWAPPED_MASK));

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
        // Page Fault. Reason: Not present
        if (handlePageFault(vab, pte) <= 0) {
            return -1;
        }

        // Re-fetch the updated page table entry.
        pte = _getPte(vab);
        assert(pte & PAGE_PRESENT_MASK);
    }

    if (_translateByEntry(address, accessMode, privileges, pte) == 0) {
        pte |= PAGE_ACCESSED_MASK;
        return _setPte(vab, pte);
    } else {
        // Page Fault. Reason: Permission violation
        // We do not handle this in the assignment.
        return -1;
    }
}

//            ^  Functions known from assignment 10 (without TLB)  ^
//-----------------------------------------------------------------------------

// Returns the VMA for the specified address. Returns NULL if the address is
// not within a VMA.
static VMA* _getVMA(uint32_t address)
{
    const size_t nvmas = sizeof(_vmas) / sizeof(VMA);
    for (size_t i = 0; i < nvmas; ++i) {
        if ((_vmas[i].virtualStart <= address) &&
            (address < _vmas[i].virtualStart + _vmas[i].length)) {
            return &_vmas[i];
        }
    }

    return NULL;
}

// (Pseudo) Returns the address of a zeroed memory page.
static uint32_t _getZeroedPage()
{
    static uint32_t physicalAddressCounter = 0x10000;

    physicalAddressCounter += 0x1000;
    return physicalAddressCounter;
}

// (Pseudo) Returns the address of the page which contains the requested file
// contents.
static uint32_t _getFilePage(const char* fileName, uint32_t inFileOffset)
{
    assert((inFileOffset & OFFSET_MASK) == 0);

    if (!strcmp("/tmp/tmpfile", fileName)) {
        return 0x3740000 + inFileOffset;
    } else if (!strcmp("/kernel", fileName)) {
        return 0x1300000 + inFileOffset;
    } else {
        printf("The file is not mapped: %s\n", fileName);
        return 0;
    }
}

// (Pseudo) Stores a page on disk and returns the new disk address.
uint32_t _storeOnDisk(uint32_t physAddress)
{
    assert((physAddress & OFFSET_MASK) == 0);
    static uint32_t diskBlockCounter = 0x1ff000;

    diskBlockCounter += 0x1000;
    return diskBlockCounter;
}

// (Pseudo) Loads a block from the disk to a memory page and returns
// the physical address.
uint32_t _loadFromDisk(uint32_t diskAddress)
{
    assert((diskAddress & OFFSET_MASK) == 0);
    // Assuming that we do not that many swap operations in our test code.
    assert(0x200000 <= diskAddress && diskAddress < 0x280000);
    static uint32_t memoryAddressCounter = 0x61f000;

    memoryAddressCounter += 0x1000;
    return memoryAddressCounter;
}

// Called whenever an access to a non-present page occurs. The handler
// should not be called for access violations.
int handlePageFault(uint32_t virtualBase, uint32_t pte)
{
    assert(_getOffset(virtualBase) == 0);
    assert(!(pte & PAGE_PRESENT_MASK));

    if (pte & PAGE_SWAPPED_MASK) {
        // The page is swapped to disk. Load it.
        uint32_t diskAddress  = _getVirtualBase(pte);
        uint32_t physicalBase = _loadFromDisk(diskAddress);

        // Set the page table entry to the physical address to where
        // we loaded the block from disk.
        pte = physicalBase | (pte & PRESERVED_BITS_ON_SWAP) | PAGE_PRESENT_MASK;
        _setPte(virtualBase, pte);

        return 1;
    }

    VMA *vma = _getVMA(virtualBase);
    if (vma) {
        uint32_t physicalBase;
        switch (vma->type) {
            case VMA_TYPE_NULL:
                physicalBase = _getZeroedPage();
                break;

            case VMA_TYPE_FILE: {
                uint32_t fileOffset = virtualBase - vma->virtualStart;
                physicalBase = _getFilePage(vma->fileName, fileOffset);
                break;
            }

            default:
                return -1;
        }

        // Since we do not have a pte with correct permissions for this page
        // we create a fresh one with mapPage.
        mapPage(virtualBase, physicalBase, vma->accessMode, vma->privileges);

        return 1;
    }

    return 0;
}

int swapOut(uint32_t virtualBase)
{
    if (_getOffset(virtualBase) != 0) {
        return -1;
    }

    uint32_t pte = _getPte(virtualBase);
    if (!(pte & PAGE_PRESENT_MASK)) {
        return -1;
    }

    assert(!(pte & PAGE_SWAPPED_MASK));

    uint32_t physicalBase = _getVirtualBase(pte);
    uint32_t diskAddress  = _storeOnDisk(physicalBase);

    // We store the disk address in the page table entry. The entry is not
    // present, so our MMU will ignore the physical address part of the PTE.
    // We preserve the access bits, so they do not need to be restored later.
    pte = diskAddress | PAGE_SWAPPED_MASK | (pte & PRESERVED_BITS_ON_SWAP);
    _setPte(virtualBase, pte);

    return 0;
}
