#define _DEFAULT_SOURCE
#include "testlib.h"
#include "barrier.h"
#include <unistd.h>
#include <omp.h>

#define THREADS 3

int main() {
	test_start("Your barrier synchronizes three threads.");
	test_plan(THREADS);

	int threadsAtBarrier1 = 0;

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
			test_equals_int(threadsAtBarrier1, THREADS, "All threads passed the barrier");
		}
		deleteBarrier(barrier);
	}

	return test_end();
}

