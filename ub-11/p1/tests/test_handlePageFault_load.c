#include "testlib.h"
#include "page_faults.h"

uint32_t _loadFromDisk(uint32_t diskAddress)
{
    if (diskAddress == 0x51000) {
        test_assert(1, "You call _loadFromDisk");
    }

    static uint32_t memoryAddressCounter = 0x61f000;

    memoryAddressCounter += 0x1000;
    return memoryAddressCounter;
}

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;
PageTable __attribute__((aligned(0x1000))) pageTable1;

int main()
{
    test_start("Your page fault handler loads swapped pages.");
    test_plan(7);

    basePageDirectory.entries[1] = pointerToInt(&pageTable1) | PAGE_PRESENT_MASK;

    pageTable1.entries[1] = 0x2000 | PAGE_PRESENT_MASK;
    pageTable1.entries[3] = 0x51000 | PAGE_SWAPPED_MASK | PAGE_READWRITE_MASK;
    pageTable1.entries[4] = 0x52000 | PAGE_SWAPPED_MASK | PAGE_USERMODE_MASK;
    pageTable1.entries[5] = 0x5000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK | PAGE_READWRITE_MASK | PAGE_ACCESSED_MASK;
    setPageDirectory(&basePageDirectory);

    test_equals_int(handlePageFault((1 * ENTRIES_PER_TABLE + 4) * (1 << OFFSET_BITS), pageTable1.entries[4]), 1, "handlePageFault succeeds");
    test_equals_int(handlePageFault((1 * ENTRIES_PER_TABLE + 3) * (1 << OFFSET_BITS), pageTable1.entries[3]), 1, "handlePageFault succeeds");

    test_equals_int64((pageTable1.entries[3] & PAGE_TABLE_ADDRESS_MASK), 0x621000, "entry 3: address is correct");
    test_equals_int64((pageTable1.entries[4] & PAGE_TABLE_ADDRESS_MASK), 0x620000, "entry 4: address is correct");

    test_equals_int64((pageTable1.entries[3] & PRESERVED_BITS_ON_SWAP), PAGE_READWRITE_MASK, "entry 3: flags are correct");
    test_equals_int64((pageTable1.entries[4] & PRESERVED_BITS_ON_SWAP), PAGE_USERMODE_MASK, "entry 4: flags are correct");

    return test_end();
}

