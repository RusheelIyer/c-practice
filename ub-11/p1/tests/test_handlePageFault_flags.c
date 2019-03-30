#include "testlib.h"
#include "page_faults.h"

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;
PageTable __attribute__((aligned(0x1000))) pageTable1;

int main()
{
    test_start("Your page fault handler sets the right access and present flags.");
    test_plan(8);

    basePageDirectory.entries[0] =
            pointerToInt(&pageTable1) | PAGE_PRESENT_MASK;

    setPageDirectory(&basePageDirectory);

    uint32_t address = 0x32000;
    test_equals_int64(translatePageTable(&address, ACCESS_READ, USER_MODE), 0,
            "translatePageTable is successful");
    address = 0x11000;
    test_equals_int64(translatePageTable(&address, ACCESS_READ, KERNEL_MODE), 0,
            "translatePageTable is successful");

    test_equals_int64((pageTable1.entries[0x11] & PAGE_PRESENT_MASK), PAGE_PRESENT_MASK,
            "Page 0x11 is present");
    test_equals_int64((pageTable1.entries[0x32] & PAGE_PRESENT_MASK), PAGE_PRESENT_MASK,
            "Page 0x32 is present");

    test_equals_int64((pageTable1.entries[0x11] & PAGE_READWRITE_MASK), PAGE_READWRITE_MASK,
            "Page 0x11 is RW");
    test_equals_int64((pageTable1.entries[0x32] & PAGE_READWRITE_MASK), 0,
            "Page 0x32 is not RW");

    test_equals_int64((pageTable1.entries[0x11] & PAGE_USERMODE_MASK), PAGE_USERMODE_MASK,
            "Page 0x11 is a user-mode page");
    test_equals_int64((pageTable1.entries[0x32] & PAGE_USERMODE_MASK), PAGE_USERMODE_MASK,
            "Page 0x32 is a user-mode page");

    return test_end();
}
