#define _XOPEN_SOURCE 600
#include "testlib.h"
#include "multi_mutex.h"
#include <pthread.h>
#include <errno.h>

#define MUTEX_C 10
#define UNLOCK_C 4

int main() {
	test_start("You abort locking and unlock all locked mutexs when a mutex cannot be locked in your multi_mutex_trylock.");

	pthread_mutex_t m[MUTEX_C];
	pthread_mutex_t *mp[MUTEX_C];
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	for (int i = 0; i < MUTEX_C; i++) {
		pthread_mutex_init(&m[i], &attr);
		mp[i] = &m[i];
	}

	pthread_mutex_lock(&m[2]);

	test_equals_int(multi_mutex_trylock(mp, UNLOCK_C), -1, "multi_mutex_trylock fails");

	for (int i = 0; i < MUTEX_C; i++) {
		test_equals_int(pthread_mutex_unlock(&m[i]), i == 2 ? 0 : EPERM, i == 2 ? "pthread_mutex_unlock succeeds for the single locked mutex" : "pthread_mutex_unlock fails for the other mutexes");
	}

	return test_end();
}

