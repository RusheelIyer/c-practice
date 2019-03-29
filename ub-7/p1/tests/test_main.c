#define _DEFAULT_SOURCE
#include "testlib.h"
#include "barrier.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>


#define THREADS 5

static void arrivedAtBarrier(int threadId, int barrierId) {
	static int atBarrier[THREADS];
	#pragma omp critical
	{
		char msg[100];
		snprintf(msg, sizeof(msg), "thread %d arrived at barrier %d", threadId, barrierId);
		test_equals_int(atBarrier[threadId], barrierId - 1, msg);
		for (int i = 0; i < THREADS; i++) {
			if (atBarrier[i] != barrierId - 1 && atBarrier[i] != barrierId) {
				test_failed_message("Barrier was left to soon, output is in wrong order.");
			}
		}
		atBarrier[threadId]++;
	}
}

int main() {
	test_start("Our provided main has the expected output.");
	test_plan(THREADS*3);

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

			enterBarrier(barrier);
			arrivedAtBarrier(i, 1);

			usleep((THREADS - i) * 20000);

			enterBarrier(barrier);
			arrivedAtBarrier(i, 2);

			usleep((THREADS - i) * 10000);

			enterBarrier(barrier);
			arrivedAtBarrier(i, 3);
		}
		deleteBarrier(barrier);
	}
	return test_end();
}

