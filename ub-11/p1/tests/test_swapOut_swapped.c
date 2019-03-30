#include "testlib.h"
#include "page_faults.h"

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;
PageTable __attribute__((aligned(0x1000))) pageTable1;

int main()
{
    test_start("Your swapOut marks the page as swapped.");
    test_plan(8);

    basePageDirectory.entries[1] = pointerToInt(&pageTable1) | PAGE_PRESENT_MASK;

    pageTable1.entries[1] = 0x2000 | PAGE_PRESENT_MASK;
    pageTable1.entries[3] = 0x3000 | PAGE_PRESENT_MASK | PAGE_READWRITE_MASK;
    pageTable1.entries[4] = 0x4000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK;
    pageTable1.entries[5] = 0x5000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK | PAGE_READWRITE_MASK;
    setPageDirectory(&basePageDirectory);

    swapOut((1 * ENTRIES_PER_TABLE + 4) * (1<<OFFSET_BITS));
    swapOut((1 * ENTRIES_PER_TABLE + 1) * (1<<OFFSET_BITS));
    swapOut((1 * ENTRIES_PER_TABLE + 5) * (1<<OFFSET_BITS));

    test_equals_int64((pageTable1.entries[1] & PAGE_SWAPPED_MASK), PAGE_SWAPPED_MASK, "Page 1 is marked as swapped");
    test_equals_int64((pageTable1.entries[3] & PAGE_SWAPPED_MASK), 0, "Page 3 is not marked as swapped");
    test_equals_int64((pageTable1.entries[4] & PAGE_SWAPPED_MASK), PAGE_SWAPPED_MASK, "Page 4 is marked as swapped");
    test_equals_int64((pageTable1.entries[5] & PAGE_SWAPPED_MASK), PAGE_SWAPPED_MASK, "Page 5 is marked as swapped");

    test_equals_int64((pageTable1.entries[1] & PAGE_PRESENT_MASK), 0, "Page 1 is not marked as present");
    test_equals_int64((pageTable1.entries[3] & PAGE_PRESENT_MASK), PAGE_PRESENT_MASK, "Page 3 is marked as present");
    test_equals_int64((pageTable1.entries[4] & PAGE_PRESENT_MASK), 0, "Page 4 is not marked as present");
    test_equals_int64((pageTable1.entries[5] & PAGE_PRESENT_MASK), 0, "Page 5 is not marked as present");

    return test_end();
}
