#define _DEFAULT_SOURCE
#include "testlib.h"
#include "tslock.h"
#include <sched.h>
#include <omp.h>
#include <unistd.h>

int sched_yield(void) {
	usleep(10);
	return 0;
}

#define THREADS 5
#define INC_PER_THREAD 10

int main() {
	test_start("Your lock works with integer overflows.");

	TicketSpinlock tslock;

	// Use openmp to spawn some threads.
	omp_set_dynamic(0);
	omp_set_num_threads(THREADS);
	for (int test = 5; test --> 0;) {
		volatile int counter = 0;
		tslock.currentTicket = 0;
		tslock.ticketCounter = 0;
		tslock_init(&tslock);

		tslock.currentTicket += 2147483642;
		tslock.ticketCounter += 2147483642;

		#pragma omp parallel for
		for (int i = 0; i < THREADS; i++) {
			for (int j = 0; j < INC_PER_THREAD; j++) {
				tslock_lock(&tslock);
				int c = counter;
				c++;
				sched_yield();
				counter = c;
				tslock_unlock(&tslock);
			}
		}

		test_equals_int(counter, INC_PER_THREAD * THREADS, "counter is correct");
	}

	return test_end();
}

