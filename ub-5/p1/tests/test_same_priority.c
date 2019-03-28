#include "testlib.h"
#include "scheduler.h"

int main() {
    test_start("You schedule two threads with the same priority the right way.");

    initScheduler();
    startThread(0, 5);
    onThreadReady(0);
    startThread(1, 5);
    onThreadReady(1);
    for (int i = 0; i < 10; i++) {
        test_equals_int(scheduleNextThread(), 0, "Next Thread: 0");
        onThreadPreempted(0);
        test_equals_int(scheduleNextThread(), 1, "Next Thread: 1");
        onThreadPreempted(1);
    }

    return test_end();
}

