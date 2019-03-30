#include "testlib.h"
#include "page_faults.h"

uint32_t _storeOnDisk(uint32_t physAddress)
{
    if (physAddress == 0x4000) {
        test_assert(1, "You call _storeOnDisk");
    }

    static uint32_t diskBlockCounter = 0x1ff000;

    diskBlockCounter += 0x1000;
    return diskBlockCounter;
}

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;
PageTable __attribute__((aligned(0x1000))) pageTable1;

int main()
{
    test_start("Your swapOut stores the right disk offset.");
    test_plan(4);

    basePageDirectory.entries[1] = pointerToInt(&pageTable1) | PAGE_PRESENT_MASK;

    pageTable1.entries[1] = 0x2000 | PAGE_PRESENT_MASK;
    pageTable1.entries[3] = 0x3000 | PAGE_PRESENT_MASK | PAGE_READWRITE_MASK;
    pageTable1.entries[4] = 0x4000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK;
    pageTable1.entries[5] = 0x5000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK | PAGE_READWRITE_MASK;
    setPageDirectory(&basePageDirectory);

    swapOut((1 * ENTRIES_PER_TABLE + 4) * (1<<OFFSET_BITS));
    swapOut((1 * ENTRIES_PER_TABLE + 1) * (1<<OFFSET_BITS));
    swapOut((1 * ENTRIES_PER_TABLE + 5) * (1<<OFFSET_BITS));

    test_equals_int64((pageTable1.entries[4] & PAGE_TABLE_ADDRESS_MASK), 0x200000, "entry 4: address is correct");
    test_equals_int64((pageTable1.entries[1] & PAGE_TABLE_ADDRESS_MASK), 0x201000, "entry 1: address is correct");
    test_equals_int64((pageTable1.entries[5] & PAGE_TABLE_ADDRESS_MASK), 0x202000, "entry 5: address is correct");

    return test_end();
}

