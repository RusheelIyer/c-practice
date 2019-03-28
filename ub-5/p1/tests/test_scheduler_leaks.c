#include "testlib.h"
#include "scheduler.h"

// Note: This test relies on LeakSanitizer to detect memory leaks. See
// Makefile.
int main() {
    test_start("You have no memory leaks in your scheduler.");

    initScheduler();
    startThread(0, 5);
    onThreadReady(0);
    startThread(1, 5);
    onThreadReady(1);
    startThread(2, 5);
    onThreadReady(2);

    for (int i = 0; i < 500; i++) {
        scheduleNextThread();
    }

    return test_end();
}
