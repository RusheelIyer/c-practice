#include "testlib.h"
#include "workerpool.h"

#include <string.h>

extern WorkItem _workItems[MAX_JOBS];
extern uint32_t _nextJob;
extern uint32_t _numJobs;

int _enqueue(WorkFunc func, int arg);
int _dequeue(WorkItem *item);

int main()
{
    test_start("You enqueue/dequeue work to/from the ring buffer in the right way.");
    test_plan(5*MAX_JOBS + 22);

    WorkItem item;
    int r;

    for (int i = 0; i < MAX_JOBS; i++) {
        r = _enqueue((WorkFunc)((intptr_t)0xbaadf000 + i), i);
        test_equals_int(r, 0, "_enqueue was successful");
        test_equals_int(_nextJob, 0, "_nextJob is 0");
        test_equals_int(_numJobs, i + 1, "_numJobs is correct");
        test_equals_ptr(_workItems[i].func, (WorkFunc)((intptr_t)0xbaadf000 + i), ".func is correct");
        test_equals_int(_workItems[i].arg, i, ".arg is correct");
    }

    // The ring buffer should be full by now. Try to add another work item.
    r = _enqueue((WorkFunc)((intptr_t)0xbaadf000 + MAX_JOBS), MAX_JOBS);
    test_equals_int(r, -1, "The ring buffer is full");

    // Now dequeue some work.
    r = _dequeue(&item);
    test_equals_int(r, 0, "_dequeue was successful");
    test_equals_int(_nextJob, 1, "_nextJob is 1");
    test_equals_int(_numJobs, MAX_JOBS - 1, "_numJobs is MAX_JOBS - 1");
    test_equals_ptr(item.func, (WorkFunc)((intptr_t)0xbaadf000 + 0), ".func is correct");
    test_equals_int(item.arg, 0, ".arg is correct");

    r = _dequeue(&item);
    test_equals_int(r, 0, "_dequeue was successful");
    test_equals_int(_nextJob, 2, "_nextJob is 2");
    test_equals_int(_numJobs, MAX_JOBS - 2, "_numJobs is MAX_JOBS - 2");
    test_equals_ptr(item.func, (WorkFunc)((intptr_t)0xbaadf000 + 1), ".func is correct");
    test_equals_int(item.arg, 1, ".arg is correct");

    // Add new work
    r = _enqueue((WorkFunc)((intptr_t)0xdeadbeef + 0), 0xf00);
    test_equals_int(r, 0, "_enqueue was successful");
    test_equals_int(_nextJob, 2, "_nextJob is 2");
    test_equals_int(_numJobs, MAX_JOBS - 1, "_numJobs is MAX_JOBS - 2");
    test_equals_ptr(_workItems[0].func, (WorkFunc)((intptr_t)0xdeadbeef + 0), ".func is correct");
    test_equals_int(_workItems[0].arg, 0xf00, ".arg is correct");

    r = _enqueue((WorkFunc)((intptr_t)0xdeadbeef + 1), 0xf01);
    test_equals_int(r, 0, "_enqueue was successful");
    test_equals_int(_nextJob, 2, "_nextJob is 2");
    test_equals_int(_numJobs, MAX_JOBS, "_numJobs is MAX_JOBS");
    test_equals_ptr(_workItems[1].func, (WorkFunc)((intptr_t)0xdeadbeef + 1), ".func is correct");
    test_equals_int(_workItems[1].arg, 0xf01, ".arg is correct");

    // The ring buffer should be full again. Try to add another work item.
    r = _enqueue((WorkFunc)((intptr_t)0xdeadbeef + 2), 0xf02);
    test_equals_int(r, -1, "The ring buffer is full");

    return test_end();
}
