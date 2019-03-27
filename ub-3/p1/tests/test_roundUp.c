#include "testlib.h"
#include "malloc.h"

int main() {
    test_start("Your roundUp works");
    test_plan(5);

    test_equals_int64(roundUp(0), 0, "0 -> 0");
    test_equals_int64(roundUp(1), 16, "1 -> 16");
    test_equals_int64(roundUp(15), 16, "15 -> 16");
    test_equals_int64(roundUp(120), 128, "128 -> 128");
    test_equals_int64(roundUp(0x283732837472llu), 0x283732837480llu, "0x283732837472 -> 0x283732837480");

    return test_end();
}
