#include "testlib.h"
#include "barrier.h"
#include <omp.h>
#include <unistd.h>
#include <stdio.h>

#define THREADS 5

int main() {
    test_start("barrier.c");

    ThreadBarrier *barrier = createBarrier(THREADS);
    if (barrier == NULL) {
        test_failed_message("barrier == NULL");
    } else {
        // Use openmp to spawn some threads.
        omp_set_dynamic(0);
        omp_set_num_threads(THREADS);
        #pragma omp parallel for
        for (int i = 0; i < THREADS; i++) {
            #pragma omp critical
            printf("Thread %d started.\n", i);

            enterBarrier(barrier);

            #pragma omp critical
            printf("Thread %d after first sync point.\n", i);

            enterBarrier(barrier);

            #pragma omp critical
            printf("Thread %d after second sync point.\n", i);
        }
        deleteBarrier(barrier);
    }

    return test_end();
}