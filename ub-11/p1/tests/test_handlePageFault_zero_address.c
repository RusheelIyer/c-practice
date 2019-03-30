#include "testlib.h"
#include "page_faults.h"

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;
PageTable __attribute__((aligned(0x1000))) pageTable1;

int main()
{
    test_start("Your page fault handler sets the right address for zeroed pages.");
    test_plan(6);

    basePageDirectory.entries[0] =
            pointerToInt(&pageTable1) | PAGE_PRESENT_MASK;

    setPageDirectory(&basePageDirectory);

    uint32_t address = 0x10000;
    test_equals_int64(translatePageTable(&address, ACCESS_READ, USER_MODE), 0,
            "translatePageTable is successful");
    test_equals_int64(address, 0x11000, "Translated address is correct");
    address = 0x11000;
    test_equals_int64(translatePageTable(&address, ACCESS_READ, KERNEL_MODE), 0,
            "translatePageTable is successful");
    test_equals_int64(address, 0x12000, "Translated address is correct");

    test_equals_int64((pageTable1.entries[0x10] & PAGE_TABLE_ADDRESS_MASK),
            0x11000,
            "Address mask is correct");
    test_equals_int64((pageTable1.entries[0x11] & PAGE_TABLE_ADDRESS_MASK),
            0x12000,
            "Address mask is correct");

    return test_end();
}
