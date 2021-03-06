#include "testlib.h"
#include "scheduler.h"

int main() {
    test_start("You schedule one thread with highest priority every time.");

    initScheduler();
    startThread(0, 5);
    onThreadReady(0);
    for (int i = 0; i < 10; i++) {
        test_equals_int(scheduleNextThread(), 0, "Next Thread: 0");
        onThreadPreempted(0);
    }

    return test_end();
}
