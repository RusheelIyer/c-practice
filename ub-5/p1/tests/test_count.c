#include "testlib.h"
#include "scheduler.h"

int main() {
    test_start("You do not miss to count threads just because they switch to the waiting state.");

    initScheduler();
    startThread(0, 5);
    onThreadReady(0);
    startThread(1, 4);

    test_equals_int(scheduleNextThread(), 0, "Next Thread: 0");
    onThreadPreempted(0);
    test_equals_int(scheduleNextThread(), 0, "Next Thread: 0");
    onThreadPreempted(0);
    test_equals_int(scheduleNextThread(), 0, "Next Thread: 0");
    onThreadPreempted(0);

    onThreadReady(1);

    test_equals_int(scheduleNextThread(), 0, "Next Thread: 0");
    onThreadPreempted(0);
    test_equals_int(scheduleNextThread(), 0, "Next Thread: 0");
    onThreadPreempted(0);

    test_equals_int(scheduleNextThread(), 1, "Next Thread: 1");
    onThreadPreempted(1);

    test_equals_int(scheduleNextThread(), 0, "Next Thread: 0");

    return test_end();
}

