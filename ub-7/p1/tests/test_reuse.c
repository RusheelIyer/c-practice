#define _DEFAULT_SOURCE
#include "testlib.h"
#include "barrier.h"
#include <unistd.h>
#include <stdlib.h>
#include <omp.h>


#define THREADS 5

int main() {
	test_start("You can use the barrier multiple times.");
	test_plan(THREADS*3);

	int threadsAtBarrier1 = 0;
	int threadsAtBarrier2 = 0;
	int threadsAtBarrier3 = 0;

	ThreadBarrier *barrier = createBarrier(THREADS);
	if (barrier == NULL) {
		test_failed_message("barrier == NULL");
	} else {
		// Use openmp to spawn some threads.
		omp_set_dynamic(0);
		omp_set_num_threads(THREADS);
		#pragma omp parallel for
		for (int i = 0; i < THREADS; i++) {
			usleep((i + 1) * 10000);

			#pragma omp critical
			threadsAtBarrier1++;
			enterBarrier(barrier);
			#pragma omp critical
			test_equals_int(threadsAtBarrier1, THREADS, "All threads passed the first barrier");

			usleep((THREADS - i) * 20000);

			#pragma omp critical
			threadsAtBarrier2++;
			enterBarrier(barrier);
			#pragma omp critical
			test_equals_int(threadsAtBarrier2, THREADS, "All threads passed the second barrier");

			usleep((THREADS - i) * 10000);

			#pragma omp critical
			threadsAtBarrier3++;
			enterBarrier(barrier);
			#pragma omp critical
			test_equals_int(threadsAtBarrier3, THREADS, "All threads passed the third barrier");
		}
		deleteBarrier(barrier);
	}
	return test_end();
}

