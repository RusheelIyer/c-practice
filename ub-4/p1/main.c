#include "testlib.h"
#include "workerpool.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static void short_job(int arg)
{
    printf("Worker <%i>: Executed short job with arg %i.\n", getWorkerId(), arg);
}

static void long_job(int arg)
{
    sleep(1);

    printf("Worker <%i>: Executed long job with arg %i.\n", getWorkerId(), arg);
}

int main()
{
    test_start("workerpool.c");
    if (initializeWorkerPool() != 0) {
        test_failed_message("Failed to initialize worker pool.")
    }

    // Submit some work for the worker pool.
    int r = submitWork(short_job, 0);
    test_equals_int(r, 0, "submitWork succeeds");

    r = submitWork(short_job, 1);
    test_equals_int(r, 0, "submitWork succeeds");

    r = submitWork(NULL, 2);
    test_equals_int(r, -1, "submitWork fails with NULL function");

    r = submitWork(long_job, 3);
    test_equals_int(r, 0, "submitWork succeeds");

    r = submitWork(short_job, 4);
    test_equals_int(r, 0, "submitWork succeeds");

    r = submitWork(short_job, 5);
    test_equals_int(r, 0, "submitWork succeeds");

    r = submitWork(long_job, 6);
    test_equals_int(r, 0, "submitWork succeeds");

    // Give the worker pool some time to process the work items before we exit.
    sleep(1);

    finalizeWorkerPool();
    return test_end();
}
