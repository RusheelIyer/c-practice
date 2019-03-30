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
    test_start("Your TLB uses LRU to evict old items.");
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

    // refresh
    address = 0x100;
    test_equals_int(translateTLB(&address), 0, "TLB contains 0x100 (refresh)");
    test_equals_int64(address, 0x200, "TLB translates 0x100 correctly");

    // this should evict.
    addToTLB(0x400, 0x600);
    address = 0x100;
    test_equals_int(translateTLB(&address), 0, "TLB still contains 0x100");
    test_equals_int64(address, 0x200, "TLB translates 0x100 correctly");
    address = 0x200;
    test_equals_int(translateTLB(&address), -1, "TLB does not contain 0x200 anymore");
    test_equals_int64(address, 0x200, "address is not changed");
    address = 0x201;
    test_equals_int(translateTLB(&address), 0, "TLB still contains 0x201");
    test_equals_int64(address, 0x400, "TLB translates 0x201 correctly");
    address = 0x300;
    test_equals_int(translateTLB(&address), 0, "TLB still contains 0x300");
    test_equals_int64(address, 0x500, "TLB translates 0x300 correctly");
    address = 0x400;
    test_equals_int(translateTLB(&address), 0, "TLB now contains 0x400");
    test_equals_int64(address, 0x600, "TLB translates 0x400 correctly");

    // This is not neccessary.
    setSegmentTable(&_table);
    // test refresh with set.
    addToTLB(0x100, 0x200);
    addToTLB(0x200, 0x300);
    addToTLB(0x201, 0x400);
    addToTLB(0x300, 0x500);
    addToTLB(0x100, 0x200);

    addToTLB(0x400, 0x600);
    address = 0x100;
    test_equals_int(translateTLB(&address), 0, "TLB still contains 0x100");
    test_equals_int64(address, 0x200, "TLB translates 0x100 correctly");
    address = 0x200;
    test_equals_int(translateTLB(&address), -1, "TLB does not contain 0x200 anymore");
    test_equals_int64(address, 0x200, "address is not changed");
    address = 0x201;
    test_equals_int(translateTLB(&address), 0, "TLB still contains 0x201");
    test_equals_int64(address, 0x400, "TLB translates 0x201 correctly");
    address = 0x300;
    test_equals_int(translateTLB(&address), 0, "TLB still contains 0x300");
    test_equals_int64(address, 0x500, "TLB translates 0x300 correctly");
    address = 0x400;
    test_equals_int(translateTLB(&address), 0, "TLB now contains 0x400");
    test_equals_int64(address, 0x600, "TLB translates 0x400 correctly");

    return test_end();
}

