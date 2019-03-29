#include "barrier.h"
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct _ThreadBarrier {
    // Number of threads this barrier waits for
    int threads;

    // Number of threads waiting.
    int waiting;

    pthread_mutex_t mutex;

    // Condition that is signaled as soon as all threads arrive at the
    // barrier.
    pthread_cond_t condition;

    // How many times the barrier has been used.
    // We use this to detect spurious wakeups.
    int stateCounter;
};

ThreadBarrier *createBarrier(int threads)
{
    if (threads < 2) {
        return NULL;
    }

    ThreadBarrier *barrier = malloc(sizeof(ThreadBarrier));
    if (barrier == NULL) {
        return NULL;
    }

    barrier->threads = threads;
    barrier->waiting = 0;
    barrier->stateCounter = 0;

    if (pthread_mutex_init(&barrier->mutex, NULL)) {
        free(barrier);
        return NULL;
    }

    if (pthread_cond_init(&barrier->condition, NULL)) {
        pthread_mutex_destroy(&barrier->mutex);
        free(barrier);
        return NULL;
    }

    return barrier;
}

void enterBarrier(ThreadBarrier *barrier)
{
    if (barrier == NULL) {
        return;
    }

    // Lock the barrier mutex. That synchronizes the access to the fields of
    // our thread barrier data structure
    pthread_mutex_lock(&barrier->mutex);

    // Check if there are still other threads (despite this one) not waiting
    // on the barrier. It is the job of the last thread that reaches the
    // barrier to NOT wait, but release all other threads.
    if (barrier->waiting < barrier->threads - 1) {
        barrier->waiting++;
        int stateCounterWaiting = barrier->stateCounter;

        // We block until barrier->stateCounter was incremented by the
        // last thread that entered the barrier.
        while (stateCounterWaiting == barrier->stateCounter) {
            // Wait on the condition. This will also atomically
            // unlock the mutex and put this thread to sleep.
            pthread_cond_wait(&barrier->condition, &barrier->mutex);
        }
    } else {
        assert(barrier->waiting > 0);

        barrier->stateCounter++;
        // Wake up all threads waiting on the barrier
        pthread_cond_broadcast(&barrier->condition);
        barrier->waiting = 0;
    }

    // When a thread comes here it holds the barrier lock. It either has been
    // woken up and thus re-acquired the lock at wakeup through the operating
    // system or the thread never entered the wait, releasing the other
    // threads, and thus never gave up the lock.
    pthread_mutex_unlock(&barrier->mutex);
}

void deleteBarrier(ThreadBarrier *barrier)
{
    if (barrier == NULL) {
        return;
    }

    pthread_cond_destroy(&barrier->condition);
    pthread_mutex_destroy(&barrier->mutex);

    free(barrier);
}
