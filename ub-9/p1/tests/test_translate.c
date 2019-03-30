#include "testlib.h"
#include "mmu.h"
#include <stdio.h>

static SegmentTable _table = {
    .segments = {
        {.base = 0, .length = 0x80},

        {.base = 0x100, .length = 0x80},

        {.base = 0x200, .length = 0x3},

        {.base = 0x300, .length = 0x10},

        // Define invalid segments by giving a 0 length.
        {.base = 0x400, .length = 0x0},

        {.base = 0x111, .length = 0x1},

        {.base = 0x800, .length = 0x100},

        {.base = 0x8ff00000, .length = 0x1ffffffe}
    }
};

void testTranslate(uint32_t from, uint32_t to) {
    uint32_t address = from;
    char msg[100];
    snprintf(msg, sizeof msg, "translating 0x%x succeeds", from);
    test_equals_int(translateSegmentTable(&address), 0, msg);
    snprintf(msg, sizeof msg, "virtual address 0x%x translates correctly", from);
    test_equals_int64(address, to, msg);
}

int main() {
    test_start("Your segment table translations translates valid addresses.");

    setSegmentTable(&_table);

    testTranslate(0, 0);
    testTranslate(0x20000000, 0x100);
    testTranslate(0x20000010, 0x110);
    testTranslate(0x2000007f, 0x17f);
    testTranslate(0x40000002, 0x202);
    testTranslate(0x60000001, 0x301);
    testTranslate(0xa0000000, 0x111);
    testTranslate(0xc0000011, 0x811);
    testTranslate(0xe0103821, 0x90003821);

    return test_end();
}

