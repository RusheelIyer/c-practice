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

static SegmentTable _table2 = {
    .segments = {
        {.base = 0, .length = 0x100},

        {.base = 0x80, .length = 0x80},

        {.base = 0x200, .length = 0x3},

        {.base = 0x300, .length = 0x10},

        // Define invalid segments by giving a 0 length.
        {.base = 0x400, .length = 0x0},

        {.base = 0x111, .length = 0x1},

        {.base = 0x800, .length = 0x100},

        {.base = 0x0, .length = 0x1ffffffe}
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
    test_start("You use a new segment table if a new one is set.");

    setSegmentTable(&_table);

    testTranslate(0, 0);
    testTranslate(0x20000000, 0x100);
    testTranslate(0xe0000000, 0x8ff00000);

    setSegmentTable(&_table2);

    testTranslate(0, 0);
    testTranslate(0x20000000, 0x80);
    testTranslate(0xe0000001, 0x1);

    return test_end();
}

