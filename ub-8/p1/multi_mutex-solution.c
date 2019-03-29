#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <alloca.h>
#include <assert.h>
#include "multi_mutex.h"

int multi_mutex_unlock(pthread_mutex_t **mutexv, int mutexc)
{
    if ((mutexv == NULL) || (mutexc < 0)) {
        return -1;
    }

    // Attempt to unlock all mutexes.
    for (int i = 0; i < mutexc; i++) {
        if (mutexv[i]) {
            // No error handling here.
            pthread_mutex_unlock(mutexv[i]);
        }
    }

    return 0;
}

int multi_mutex_trylock(pthread_mutex_t **mutexv, int mutexc)
{
    if ((mutexv == NULL) || (mutexc < 0)) {
        return -1;
    }

    for (int i = 0; i < mutexc; i++) {
        if (mutexv[i]) {
            int r = pthread_mutex_trylock(mutexv[i]);
            if (r == 0) {
                continue;
            }
        }

        // A lock error occurred.
        // We should unlock the already locked mutexes.
        multi_mutex_unlock(mutexv, i);

        return -1;
    }

    return 0;
}

static int _compare_pointer(const void *a, const void *b)
{
    assert(a != NULL);
    assert(b != NULL);

    // Qsort passes a pointer to the two array elements to compare.
    // Each array element is a pthread_mutex_t*.
    pthread_mutex_t **mutex_a = (pthread_mutex_t **) a;
    pthread_mutex_t **mutex_b = (pthread_mutex_t **) b;

    // mutex_a points to a pointer to the first mutex.
    // If we follow it once, we get the pointer to the mutex.
    // This pointer is the memory address we want for comparison.
    return (*mutex_a < *mutex_b) ? -1 : ((*mutex_a == *mutex_b) ? 0 : 1);
}

#define ALLOCA_MAX 100

int multi_mutex_lock(pthread_mutex_t **mutexv, int mutexc)
{
    if ((mutexv == NULL) || (mutexc < 0)) {
        return -1;
    } else if (mutexc == 0) {
        return 0;
    }

    // We must lock the mutexes according to their location in memory. We hence
    // must sort the input array. The assignment does not specify if we are
    // allowed to modify the input mutex array. It is good practice to not
    // alter the application's state if the function may still fail. To achieve
    // this we should not change the mutex directly, but copy it. If the array
    // is small enough, we allocate it on the stack.
    size_t size = sizeof(pthread_mutex_t*) * mutexc;
    pthread_mutex_t **mutexv_temp;
    mutexv_temp = (size > ALLOCA_MAX) ? malloc(size) : alloca(size);

    if (mutexv_temp == NULL) {
        return -1;
    }

    memcpy(mutexv_temp, mutexv, size);

    // Sort the mutexes and then lock them. Note: The assignment does not
    // ask us to validate the array elements. However, we will do it for
    // improved error checking.
    qsort(mutexv_temp, mutexc, sizeof(pthread_mutex_t*), _compare_pointer);

    pthread_mutex_t *prev = NULL;
    for (int i = 0; i < mutexc; prev = mutexv_temp[i++]) {
        assert(prev <= mutexv_temp[i]);
        if ((prev != mutexv_temp[i]) && mutexv_temp[i]) {
            int r = pthread_mutex_lock(mutexv_temp[i]);
            if (r == 0) {
                continue;
            }
        }

        // A lock error occurred.
        // We should unlock the already locked mutexes.
        multi_mutex_unlock(mutexv_temp, i);

        if (size > ALLOCA_MAX) {
            free(mutexv_temp);
        }

        return -1;
    }

    // If we used malloc, we need to free the memory.
    if (size > ALLOCA_MAX) {
        free(mutexv_temp);
    }

    return 0;
}
