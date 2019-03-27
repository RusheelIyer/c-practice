#include "testlib.h"
#include "persistence.h"
#include <stdio.h>

static struct tm statictime;

void* __wrap_malloc(size_t size) {
    test_equals_int64((int64_t) size, (int64_t) sizeof(struct tm), "You call malloc with the correct size");
    return &statictime;
}

int main() {
    test_start("You call malloc and set the address correctly.");
    test_plan(2);

    time_t timecode1 = 1420066800;
    struct tm* time1 = gmtime(&timecode1);
    make_persistent(&time1);

    test_equals_ptr(time1, &statictime, "You set the address correctly");

    return test_end();
}
