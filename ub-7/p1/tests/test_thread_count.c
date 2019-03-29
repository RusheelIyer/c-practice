#include "testlib.h"
#include "barrier.h"
#include <stdlib.h>

int main() {
    test_start("A barrier is not created if thread count does not make sense (<2).");
	test_plan(2);

    ThreadBarrier *barrier = createBarrier(1);
    test_equals_ptr(barrier, NULL, "createBarrier(1) return NULL");

    barrier = createBarrier(-1);
    test_equals_ptr(barrier, NULL, "createBarrier(-1) return NULL");

    return test_end();
}
