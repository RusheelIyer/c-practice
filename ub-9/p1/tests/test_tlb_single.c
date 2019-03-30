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
    test_start("Your TLB can store a single address translation.");
    uint32_t address;

    setSegmentTable(&_table);

    address = 0x100;
    test_equals_int(translateTLB(&address), -1, "TLB does not contain 0x100");
    test_equals_int64(address, 0x100, "address is not changed");
    addToTLB(0x100, 0x200);
    address = 0x100;
    test_equals_int(translateTLB(&address), 0, "TLB contains 0x100");
    test_equals_int64(address, 0x200, "TLB translates 0x100 correctly");

    return test_end();
}

