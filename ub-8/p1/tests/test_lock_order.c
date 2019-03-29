#define _XOPEN_SOURCE 600
#include "testlib.h"
#include "multi_mutex.h"
#include <pthread.h>
#include <errno.h>
#include <stdio.h>

#define MUTEX_C 10
#define UNLOCK_C 4

pthread_mutex_t *lastMutex = NULL;
int mutexCount = 0;

int __real_pthread_mutex_lock(pthread_mutex_t *mutex);
int __wrap_pthread_mutex_lock(pthread_mutex_t *mutex) {
	char msg[100];
	snprintf(msg, sizeof msg, "You lock mutex %d in the right order", mutexCount++);
	test_assert(mutex > lastMutex, msg);
	lastMutex = mutex;

	return __real_pthread_mutex_lock(mutex);
}

int main() {
	test_start("You lock the mutexes in the right order in your multi_mutex_lock.");
	test_plan(MUTEX_C + UNLOCK_C);

	pthread_mutex_t m[MUTEX_C];
	pthread_mutex_t *mp[MUTEX_C];
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	for (int i = 0; i < MUTEX_C; i++) {
		pthread_mutex_init(&m[i], &attr);
		mp[i] = &m[i];
	}

	multi_mutex_lock(mp, UNLOCK_C);

	for (int i = 0; i < UNLOCK_C; i++) {
		test_equals_int(pthread_mutex_unlock(&m[i]), 0, "pthread_mutex_unlock succeeds");
	}

	for (int i = UNLOCK_C; i < MUTEX_C; i++) {
		test_equals_int(pthread_mutex_unlock(&m[i]), EPERM, "pthread_mutex_unlock fails with EPERM");
	}

	return test_end();
}

