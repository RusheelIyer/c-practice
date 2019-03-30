#include "testlib.h"
#include "mmu.h"

static SegmentTable _table = {
    .segments = {
        {.base = 0, .length = 0x1},
        {.base = 0, .length = 0x1},
        {.base = 0, .length = 0x1},
        {.base = 0, .length = 0x1},
        {.base = 0, .length = 0x1},
        {.base = 0, .length = 0x1},
        {.base = 0, .length = 0x1},
        {.base = 0, .length = 0x1}
    }
};

int main() {
    test_start("Your TLB stores 4 address translations.");
    uint32_t address;

    setSegmentTable(&_table);

    addToTLB(0x100, 0x200);
    addToTLB(0x200, 0x300);
    addToTLB(0x201, 0x400);
    addToTLB(0x300, 0x500);
    address = 0x100;
    test_equals_int(translateTLB(&address), 0, "TLB contains 0x100");
    test_equals_int64(address, 0x200, "TLB translates 0x100 correctly");
    address = 0x200;
    test_equals_int(translateTLB(&address), 0, "TLB contains 0x200");
    test_equals_int64(address, 0x300, "TLB translates 0x200 correctly");
    address = 0x201;
    test_equals_int(translateTLB(&address), 0, "TLB contains 0x201");
    test_equals_int64(address, 0x400, "TLB translates 0x201 correctly");
    address = 0x300;
    test_equals_int(translateTLB(&address), 0, "TLB contains 0x300");
    test_equals_int64(address, 0x500, "TLB translates 0x300 correctly");

    return test_end();
}

