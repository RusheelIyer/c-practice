#include "testlib.h"
#include "page_faults.h"

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;
PageTable __attribute__((aligned(0x1000))) pageTable1;

int main()
{
    test_start("Your swapOut preserves the access bits.");
    test_plan(4);

    basePageDirectory.entries[1] =
            pointerToInt(&pageTable1) | PAGE_PRESENT_MASK;

    pageTable1.entries[1] = 0x2000 | PAGE_PRESENT_MASK;
    pageTable1.entries[3] = 0x3000 | PAGE_PRESENT_MASK | PAGE_READWRITE_MASK;
    pageTable1.entries[4] = 0x4000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK;
    pageTable1.entries[5] = 0x5000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK
            | PAGE_READWRITE_MASK | PAGE_ACCESSED_MASK;
    setPageDirectory(&basePageDirectory);

    swapOut((1 * ENTRIES_PER_TABLE + 4) * (1 << OFFSET_BITS));
    swapOut((1 * ENTRIES_PER_TABLE + 1) * (1 << OFFSET_BITS));
    swapOut((1 * ENTRIES_PER_TABLE + 5) * (1 << OFFSET_BITS));

    test_equals_int64((pageTable1.entries[1] & PRESERVED_BITS_ON_SWAP), 0,
            "Page 1 has access bits preserved");
    test_equals_int64((pageTable1.entries[3] & PRESERVED_BITS_ON_SWAP),
            PAGE_READWRITE_MASK,
            "Page 3 has access bits preserved");
    test_equals_int64((pageTable1.entries[4] & PRESERVED_BITS_ON_SWAP),
            PAGE_USERMODE_MASK,
            "Page 4 has access bits preserved");
    test_equals_int64((pageTable1.entries[5] & PRESERVED_BITS_ON_SWAP),
            PAGE_USERMODE_MASK | PAGE_READWRITE_MASK | PAGE_ACCESSED_MASK,
            "Page 5 has access bits preserved");

    return test_end();
}
