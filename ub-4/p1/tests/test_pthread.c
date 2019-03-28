#include "testlib.h"
#include "workerpool.h"

#include <string.h>
#include <pthread.h>

static int _numThreads = 0;
static int _numJoins = 0;

void* _workerMain(void *arg);

int __wrap_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine) (void *), void *arg)
{
    (void)thread;
    (void)attr;

    test_equals_ptr(start_routine, _workerMain, "thread function is _workerMain");
    test_equals_int((int)((intptr_t)arg), _numThreads, "thread argument is job number");

    _numThreads++;

    // Creating the last thread fails!
    if (_numThreads == 4) {
        return -1;
    } else {
        *thread = _numThreads - 1;
        return 0;
    }
}

int __wrap_pthread_join(pthread_t thread, void **retval)
{
    (void)retval;

    test_assert((int)thread < _numThreads, "thread id in pthread_join is valid");

    _numJoins++;
    return 0;
}

int main() {
    test_start("You call pthread_create() and pthread_join() as requested.");
    // 4*pthread_create, 3*pthread_join, 1*main
    test_plan(4*2 + 3 + 1);

    test_equals_int(initializeWorkerPool(), -1, "initializeWorkerPool fails due to failing pthread_create");

    return test_end();
}
