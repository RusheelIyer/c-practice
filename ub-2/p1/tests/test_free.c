#include "testlib.h"
#include "persistence.h"
#include <stdlib.h>

static struct tm *time1;

void __wrap_free(void *ptr) {
    test_equals_ptr(ptr, time1, "You call free correctly");
}

int main() {
    test_start("Your free works correctly.");
    test_plan(2);

    time_t timecode1 = 1388530800;
    time1 = gmtime(&timecode1);
    make_persistent(&time1);
    free_persistent(&time1);

    test_assert(time1 == NULL, "free_persistent sets the pointer to NULL");

    return test_end();
}
