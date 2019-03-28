#include "testlib.h"
#include "scheduler.h"
#include <stdio.h>

int scheduled;

#define test_scheduleNextThread(should) ({ \
    scheduled = scheduleNextThread(); \
    char msg[100]; \
    snprintf(msg, sizeof(msg), "%s line %d: Next Thread: %d", __FILE__, __LINE__, should); \
    test_equals_int(scheduled, should, msg); \
})

int main() {
    test_start("You schedule the extended test scenario the right way.");

    initScheduler();

    startThread(0, 5);
    startThread(1, 4);
    startThread(2, 2);
    startThread(3, 4);
    startThread(4, 3);
    startThread(5, 4);
    startThread(6, 2);
    startThread(7, 0);

    test_scheduleNextThread(-1);
    // Let's assume we idle some time.
    onThreadReady(1);
    test_scheduleNextThread(1);
    // Let's assume Thread 1 is running here.
    onThreadPreempted(1);
    test_scheduleNextThread(1);
    // Let's assume Thread 1 is still running here.
    // Thread 1 decides to do some Disk IO.
    onThreadWaiting(1);
    test_scheduleNextThread(-1);
    // Assume we idle some time.
    // Suddenly, thread 1 gets the data from the disk.
    onThreadReady(1);
    test_scheduleNextThread(1);
    // Now thread 1 is running.
    // Thread 0 and 7 get ready.
    onThreadReady(0);
    onThreadReady(7);
    // Thread 1 is still running and gets preempted.
    for (int j = 0; j < 20; j++) {
        for (int i = 0; i < 5; i++) {
            onThreadPreempted(scheduled);
            test_scheduleNextThread(0);
        }
        onThreadPreempted(scheduled);
        test_scheduleNextThread(j % 6 == 4 ? 7 : 1);
    }

    return test_end();
}

