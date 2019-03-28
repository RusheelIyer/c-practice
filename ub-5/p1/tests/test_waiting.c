#include "testlib.h"
#include "scheduler.h"

int main() {
    test_start("You handle waiting threads the right way.");

    initScheduler();
    startThread(0, 5);
    onThreadReady(0);
    startThread(1, 4);
    onThreadReady(1);

    test_equals_int(scheduleNextThread(), 0, "Next Thread: 0");
    onThreadWaiting(0);

    test_equals_int(scheduleNextThread(), 1, "Next Thread: 1");
    onThreadWaiting(1);
    onThreadReady(0);
    test_equals_int(scheduleNextThread(), 0, "Next Thread: 0");
    onThreadWaiting(0);

    onThreadReady(1);
    onThreadReady(0);
    test_equals_int(scheduleNextThread(), 0, "Next Thread: 0");

    return test_end();
}

