#include "workerpool.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <assert.h>
#include <unistd.h>

/*
 * Indicates if the worker threads should exit. If done is 0 the workers
 * should continue to wait for/process work. Otherwise, the workers should
 * exit.
 */
static volatile int _done = 1;

/*
 * Ringbuffer to hold jobs for the workers
 */
WorkItem _workItems[MAX_JOBS];
uint32_t _nextJob = 0;
uint32_t _numJobs = 0;

/*
 * A list of worker threads. The list is initialized at runtime to hold as
 * many threads as CPUs are available in the system, but at least two.
 */
static pthread_t *_workers  = NULL;
static uint32_t _numWorkers = 0;

/*
 * The id of the current thread. The id is initialized by the workers main
 * routine at the beginning. The __thread specifier will place the variable
 * into thread local storage (TLS). That means, that each thread will have its
 * own instance of the variable.
 */
static __thread int _workerId;

/*
 * Condition variable for synchronization of worker threads. Synchronization
 * will be covered in the lecture at a later point.
 */
static pthread_mutex_t _cs;
static pthread_cond_t _cv;

#define Barrier() \
    __asm__ __volatile__ ("" ::: "memory")
#define Lock(cs) \
    pthread_mutex_lock(&(cs));
#define Unlock(cs) \
    pthread_mutex_unlock(&(cs))
#define Wait(cv, cs) \
    pthread_cond_wait(&(cv), &(cs))
#define Signal(cv) \
    pthread_cond_signal(&(cv))
#define Broadcast(cv) \
    pthread_cond_broadcast(&(cv))

/*
 * Append new work to the ringbuffer.
 * Returns -1 on error.
 */
int _enqueue(WorkFunc func, int arg)
{
    assert(func != NULL);
    assert(_nextJob < MAX_JOBS);

    if (_numJobs == MAX_JOBS) {
        return -1;
    }

    uint32_t nextEmpty = (_nextJob + _numJobs) % MAX_JOBS;
    assert(_workItems[nextEmpty].func == NULL);

    _workItems[nextEmpty].func = func;
    _workItems[nextEmpty].arg  = arg;

    _numJobs++;

    return 0;
}

/*
 * Receives work from the ringbuffer.
 * Returns -1 if no work is available.
 */
int _dequeue(WorkItem *item)
{
    assert(item != NULL);

    if (_numJobs == 0) {
        return -1;
    }

    assert(_nextJob < MAX_JOBS);
    *item = _workItems[_nextJob];

#ifndef NDEBUG
    _workItems[_nextJob].func = NULL; // Reset just for debugging
#endif

    _nextJob = (_nextJob + 1) % MAX_JOBS;
    _numJobs--;

    return 0;
}

/*
 * Blocks the current thread until there is new work or the thread should exit.
 * Returns 0 if the thread should exit.
 */
int _waitForWork(WorkItem *item)
{
    // We use the condition variable to synchronize access to the work list.
    // We wake up a new thread in submitWork() if new work is available. This
    // will get us out of Wait(). However, it might happen that another worker
    // thread, which is still running due to previous work, will be faster and
    // steal the work. In that case, we might end up empty here and immediately
    // go to sleep again with Wait().
    Lock(_cs);
    while ((!_done) && (_dequeue(item) == -1)) {
        Wait(_cv, _cs);
    }
    Unlock(_cs);

    return !_done;
}

/*
 * Main routine of the worker threads.
 * Always returns NULL.
 */
void* _workerMain(void *arg)
{
    // Initialize the thread local worker id variable.
    _workerId = (int)((intptr_t)arg);

    WorkItem item;
    while (_waitForWork(&item)) {
        assert(item.func != NULL);

        // Call the job function with the supplied argument
        item.func(item.arg);
    }

    return NULL; // Will implicitly call pthread_exit() with NULL;
}

/*
 * Starts a specified number of worker threads.
 * Returns -1 on error, 0 otherwise.
 */
static int _startWorkers(uint32_t num)
{
    assert(_workers != NULL);
    assert(_numWorkers == 0);

    for (_numWorkers = 0; _numWorkers < num; ++_numWorkers) {
        int r = pthread_create(&_workers[_numWorkers], NULL, _workerMain,
                               (void*)((intptr_t)_numWorkers));
        if (r != 0) {
            return -1;
        }
    }

    return 0;
}

/*
 * Waits for all worker threads to finish. This does not guarantee that all
 * work has been processed!
 */
static void _waitForWorkers(void)
{
    if (_workers == NULL) {
        return;
    }

    // _numWorkers will always hold the number of successfully initialized
    // workers, irrespective of the number of threads that we wanted to create.
    // This way, we can wait for the (subset of) workers we already created if
    // we encounter an error during the initialization of a worker.
    for (; _numWorkers > 0; --_numWorkers) {
        pthread_join(_workers[_numWorkers - 1], NULL);
    }
}

/*
 * Initializes the worker pool. Must be called before any other routine.
 * Returns -1 on error, 0 otherwise.
 */
int initializeWorkerPool(void)
{
    assert(_done);

    // The main thread should not be part of the worker pool and thus does not
    // get a valid id.
    _workerId = -1;

    memset(_workItems, 0, sizeof(_workItems));
    if (pthread_mutex_init(&_cs, NULL) != 0) {
        return -1;
    }

    if (pthread_cond_init(&_cv, NULL) != 0) {
        return -1;
    }

    // Get the number of processors currently available and allocate our worker
    // array as appropriately.
    long ncpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (ncpus == -1) {
        return -1;
    }

    uint32_t n = (ncpus < 4) ? 4 : ncpus;
    _workers = (pthread_t*)malloc(sizeof(pthread_t) * n);
    if (_workers == NULL) {
        goto error;
    }

    // Denote the future workers that they should not exit right away, but
    // wait for work. We use a software barrier to prevent the compiler from
    // reordering this operation beyond the barrier.
    _done = 0;
    Barrier();

    // Create the new workers
    if (_startWorkers(n) != 0) {
        goto error;
    }

    return 0;
error:
    finalizeWorkerPool();
    return -1;
}

/*
 * Finalizes the worker pool. Must be called when the worker pool is not longer
 * needed.
 */
void finalizeWorkerPool(void)
{
    assert(!_done);
    assert(_workerId == -1);

    // Inform the workers that they should stop. We use a software barrier to
    // prevent the compiler from reordering this operation beyond the barrier.
    _done = 1;
    Barrier();

    // Wake up sleeping workers so they also notice that they should exit.
    // Note that the assignment does not dictate that the worker threads should
    // process all work items before exiting.
    Broadcast(_cv);

    _waitForWorkers();

    // All workers should have ended at this point. Clean up.
    pthread_cond_destroy(&_cv);

    assert(_numWorkers == 0);
    if (_workers != NULL) {
        free(_workers);

        _workers = NULL;
    }
}

/*
 * Adds the given work to the work list. The work item is processed
 * asynchronously by one of the worker threads.
 * Returns -1 on error, 0 otherwise.
 */
int submitWork(WorkFunc func, int arg)
{
    int r;

    assert(!_done);

    if (func == NULL) {
        return -1;
    }

    // Add the new work to our work list
    Lock(_cs) {
        r = _enqueue(func, arg);
    } Unlock(_cs);

    // Wake up a single worker thread
    if (r == 0) {
        Signal(_cv);
    }

    return r;
}

/*
 * Returns the worker id of the current thread. This should always be -1 for
 * the main thread.
 */
int getWorkerId(void)
{
    return _workerId;
}