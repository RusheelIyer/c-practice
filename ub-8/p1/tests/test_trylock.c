#define _XOPEN_SOURCE 600
#include "testlib.h"
#include "multi_mutex.h"
#include <pthread.h>
#include <errno.h>

#define MUTEX_C 10
#define UNLOCK_C 4

int main() {
	test_start("You lock all mutexes in your multi_mutex_trylock.");

	pthread_mutex_t m[MUTEX_C];
	pthread_mutex_t *mp[MUTEX_C];
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	for (int i = 0; i < MUTEX_C; i++) {
		pthread_mutex_init(&m[i], &attr);
		mp[i] = &m[i];
	}

	test_equals_int(multi_mutex_trylock(mp, UNLOCK_C), 0, "multi_mutex_trylock succeeds");

	for (int i = 0; i < UNLOCK_C; i++) {
		test_equals_int(pthread_mutex_unlock(&m[i]), 0, "pthread_mutex_unlock succeeds");
	}

	for (int i = UNLOCK_C; i < MUTEX_C; i++) {
		test_equals_int(pthread_mutex_unlock(&m[i]), EPERM, "pthread_mutex_unlock fails with EPERM");
	}


	return test_end();
}

