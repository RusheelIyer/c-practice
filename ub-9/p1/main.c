#include "testlib.h"
#include "mmu.h"
#include <stdio.h>

static SegmentTable _table = {
    .segments = {
        {.base = 0, .length = 0x80},

        {.base = 0x80, .length = 0x80},

        {.base = 0x180, .length = 0x3},

        {.base = 0x190, .length = 0x1},

        // Define invalid segments by giving a 0 length.
        {.base = 0x0, .length = 0x0},

        {.base = 0x200, .length = 0x1},

        {.base = 0x800, .length = 0x100},

        {.base = 0x0, .length = 0x0}
    }
};

#define INVALID_ADDRESS ((uint32_t) -1)

static uint32_t translateAddress(uint32_t virtual)
{
    uint32_t address = virtual;

    // This is how an MMU would do the address conversion.
    // At first, attempt to find the address in the MMU.
    if (translateTLB(&address) == 0) {
        printf("Translated (using tlb): %08x --> %08x\n", virtual, address);
        // TLB lookup successful. We return that address.
        return address;
    } else if (translateSegmentTable(&address) == 0) {
        printf("Translated            : %08x --> %08x\n", virtual, address);
        // We could find a segment for that address.
        // The result is stored in the TLB so that the segment lookup
        // does not need to be done next time.
        addToTLB(virtual, address);
        return address;
    } else {
        printf("Translated            : %08x --> <invalid>\n", virtual);
        // The virtual address is invalid.
        // Return a special flag if the address is invalid.
        return INVALID_ADDRESS;
    }
}

int main()
{
    test_start("mmu.c");

    setSegmentTable(&_table);

    // Segment 0
    test_equals_int(translateAddress(0x00000000), 0x0, "virtual address 0x00000000 translates correctly");
    test_equals_int(translateAddress(0x00000070), 0x70, "virtual address 0x00000070 translates correctly");
    test_equals_int(translateAddress(0x00000800), INVALID_ADDRESS, "virtual address 0x00000800 is invalid");
    // Segment 1
    test_equals_int(translateAddress(0x20000000), 0x80, "virtual address 0x20000000 translates correctly");
    test_equals_int(translateAddress(0x20000800), INVALID_ADDRESS, "virtual address 0x20000800 is invalid");
    // Segment 5
    test_equals_int(translateAddress(0x80000000), INVALID_ADDRESS, "virtual address 0x80000000 is invalid");
    test_equals_int(translateAddress(0x80736a51), INVALID_ADDRESS, "virtual address 0x80736a51 is invalid");
    test_equals_int(translateAddress(0x9fffffff), INVALID_ADDRESS, "virtual address 0x9fffffff is invalid");

    // This should use the TLB
    test_equals_int(translateAddress(0x00000070), 0x70, "virtual address 0x00000070 translates correctly (with TLB)");
    test_equals_int(translateAddress(0x20000000), 0x80, "virtual address 0x20000000 translates correctly (with TLB)");

    // Some more tests
    test_equals_int(translateAddress(0x00000006), 0x6, "virtual address 0x00000006 translates correctly");
    test_equals_int(translateAddress(0x00000017), 0x17, "virtual address 0x00000017 translates correctly");
    test_equals_int(translateAddress(0x00000075), 0x75, "virtual address 0x00000075 translates correctly");
    test_equals_int(translateAddress(0xe0002837), INVALID_ADDRESS, "virtual address 0xe0002837 is invalid");

    // This should still be in the TLB
    test_equals_int(translateAddress(0x20000000), 0x80, "virtual address 0x20000000 translates correctly (with TLB)");
    // This should not be in the TLB
    test_equals_int(translateAddress(0x00000070), 0x70, "virtual address 0x00000070 translates correctly (not with TLB)");
    // This should use the TLB
    test_equals_int(translateAddress(0x00000075), 0x75, "virtual address 0x00000075 translates correctly (with TLB)");
    test_equals_int(translateAddress(0x00000017), 0x17, "virtual address 0x00000017 translates correctly (with TLB)");

    return test_end();
}
