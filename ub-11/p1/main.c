#include "testlib.h"
#include "page_faults.h"
#include <stdio.h>

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;

#define WRONG_ADDRESS ((uint32_t) -1)

// This can help you debugging...
static void _dumpPageTable(PageTable *table)
{
    for (int i = 0; i < ENTRIES_PER_TABLE; i++) {
        if ((table->entries[i] & (PAGE_PRESENT_MASK | PAGE_SWAPPED_MASK)) == 0) {
            continue;
        }

        char swapped  = table->entries[i] & PAGE_SWAPPED_MASK ? 'S' : ' ';
        char rw       = table->entries[i] & PAGE_READWRITE_MASK ? 'W' : ' ';
        char mode     = table->entries[i] & PAGE_USERMODE_MASK ? 'U' : ' ';
        char accessed = table->entries[i] & PAGE_ACCESSED_MASK ? 'A' : ' ';
        char present  = table->entries[i] & PAGE_PRESENT_MASK ? 'P' : ' ';

        uint32_t address = table->entries[i] & PAGE_TABLE_ADDRESS_MASK;

        printf("   Entry %04d: Page base: 0x%08"PRIx32" %c %c %c %c %c\n",
                i, address, swapped, accessed, mode, rw, present);
    }
}

static void _dumpPageDirectory(PageDirectory *dir)
{
    for (int i = 0; i < ENTRIES_PER_TABLE; i++) {
        if ((dir->entries[i] & PAGE_PRESENT_MASK) == 0) {
            continue;
        }

        uint64_t address = dir->entries[i] & PAGE_DIRECTORY_ADDRESS_MASK;

        printf("%04d => Page table at 0x%016"PRIx64"\n", i, address);
        _dumpPageTable((PageTable*)intToPointer(address));
    }
}

static uint32_t _doAddressConversion(uint32_t virtual, ReadWrite accessMode,
    PrivilegeLevel privileges)
{
    char rw   = (accessMode == ACCESS_WRITE) ? 'W' : 'R';
    char mode = (privileges == KERNEL_MODE) ? 'K' : 'U';

    uint32_t address = virtual;

    if (translatePageTable(&address, accessMode, privileges) == 0) {
        printf("Translated: 0x%08"PRIx32" (%c,%c) --> 0x%08"PRIx32"\n",
                virtual, rw, mode, address);

        return address;       // We found a valid translation
    } else {
        printf("Translated: 0x%08"PRIx32" (%c,%c) --> <invalid>\n",
                virtual, rw, mode);

        return WRONG_ADDRESS; // The address does not have a valid mapping.
    }
}

int main()
{
    test_start("page_faults.c");

    setPageDirectory(&basePageDirectory);

    // Access some nulled pages.
    // They should be mapped to the fake addresses.
    test_equals_int64(_doAddressConversion(0x10000, ACCESS_READ, USER_MODE), 0x11000, "Read from nulled page");
    test_equals_int64(_doAddressConversion(0x11000, ACCESS_READ, KERNEL_MODE), 0x12000, "Read from nulled page");
    test_equals_int64(_doAddressConversion(0x16000, ACCESS_READ, USER_MODE), 0x13000, "Read from nulled page");
    test_equals_int64(_doAddressConversion(0x11000, ACCESS_WRITE, KERNEL_MODE), 0x12000, "Write to nulled page");

    // Access the files. They should be mapped to our fake file pages.
    test_equals_int64(_doAddressConversion(0x30000, ACCESS_READ, USER_MODE), 0x3740000, "Read from file");
    test_equals_int64(_doAddressConversion(0x33000, ACCESS_WRITE, KERNEL_MODE), 0x3743000, "Write to file");
    test_equals_int64(_doAddressConversion(0x8563000, ACCESS_READ, KERNEL_MODE), 0x1300000, "Read from file");
    test_equals_int64(_doAddressConversion(0x8565000, ACCESS_WRITE, KERNEL_MODE), 0x1302000, "Write to file");

    // This access sould fail.
    test_equals_int64(_doAddressConversion(0x33000, ACCESS_WRITE, USER_MODE), WRONG_ADDRESS, "Write to invalid address should fail");
    test_equals_int64(_doAddressConversion(0x8564000, ACCESS_READ, USER_MODE), WRONG_ADDRESS, "Read from invalid address should fail");
    test_equals_int64(_doAddressConversion(0x8564000, ACCESS_READ, KERNEL_MODE), 0x1301000, "Read from kernel page should fail");

    printf("\n");
    printf("Your page table after the first access sequence (should contain 8 entries):\n");
    _dumpPageDirectory(&basePageDirectory);

    // Let's swap out some random pages:
    swapOut(0x8564000);
    swapOut(0x8563000);
    swapOut(0x16000);
    swapOut(0x10000);
    swapOut(0x11000);

    printf("\n");
    printf("Your page table after swapping out some pages:\n");
    _dumpPageDirectory(&basePageDirectory);
    printf("\n");
    test_equals_int64(_doAddressConversion(0x10000, ACCESS_READ, USER_MODE), 0x620000, "Read should work");
    test_equals_int64(_doAddressConversion(0x11000, ACCESS_READ, USER_MODE), 0x621000, "Read should work");
    test_equals_int64(_doAddressConversion(0x8564000, ACCESS_READ, USER_MODE), WRONG_ADDRESS, "Read should fail");
    test_equals_int64(_doAddressConversion(0x8564000, ACCESS_READ, KERNEL_MODE), 0x622000, "Kernel mode read should work");
    test_equals_int64(_doAddressConversion(0x10000, ACCESS_READ, USER_MODE), 0x620000, "Read should work");
    test_equals_int64(_doAddressConversion(0x11000, ACCESS_READ, USER_MODE), 0x621000, "Read should work");

    printf("\n");
    printf("Your page table after re-accessing some pages:\n");
    _dumpPageDirectory(&basePageDirectory);

    return test_end();
}