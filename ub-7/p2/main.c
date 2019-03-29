#include "testlib.h"
#include "tslock.h"
#include <omp.h>
#include <stdio.h>

#define THREADS 10
// Decrease this value if the test takes to long for you.
#define INC_PER_THREAD 100000

int main()
{
    test_start("tslock.c");
    TicketSpinlock tslock;
    int counter = 0;

    tslock_init(&tslock);

    // Use openmp to spawn some threads.
    omp_set_dynamic(0);
    omp_set_num_threads(THREADS);
    #pragma omp parallel for
    for (int i = 0; i < THREADS; i++) {
        for (int j = 0; j < INC_PER_THREAD; j++) {
            tslock_lock(&tslock);
            counter++;
            tslock_unlock(&tslock);
        }
    }

    test_equals_int(counter, INC_PER_THREAD * THREADS, "counter was incremented correctly");

    return test_end();
}
