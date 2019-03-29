#include <alloca.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "multi_mutex.h"

int multi_mutex_unlock(pthread_mutex_t** mutexv, int mutexc) {
  if (mutexv == NULL)
    return -1;

  for (int i = 0; i < mutexc; i++) {
    if (pthread_mutex_unlock(*(mutexv + i)) != 0)
      return -1;
  }

  return 0;
}

int multi_mutex_trylock(pthread_mutex_t** mutexv, int mutexc) {
  if (mutexv == NULL)
    return -1;

  for (int i = 0; i < mutexc; i++) {
    if (pthread_mutex_lock(*(mutexv + i)) != 0) {
      multi_mutex_unlock(mutexv, i + 1);
      return -1;
    }
  }

  return 0;
}

int multi_mutex_lock(pthread_mutex_t** mutexv, int mutexc) {
  if (mutexv == NULL)
    return -1;

  pthread_mutex_t** sorted = malloc(sizeof(pthread_mutex_t*) * mutexc);

  if (sorted == NULL) {
    return -1;
  }

  memcpy(sorted, mutexv, sizeof(pthread_mutex_t*) * mutexc);

  qsort(sorted, mutexc, sizeof(pthread_mutex_t*), _compare_pointer);

  for (int i = 0; i < mutexc; i++) {
    if (pthread_mutex_lock(*(sorted + i)) != 0) {
      multi_mutex_unlock(sorted, i + 1);
      return -1;
    }
  }

  return 0;
}

static int _compare_pointer(const void* a, const void* b) {
  // Qsort passes a pointer to the two array elements to compare.
  // Each array element is a pthread_mutex_t*.
  pthread_mutex_t** mutex_a = (pthread_mutex_t**)a;
  pthread_mutex_t** mutex_b = (pthread_mutex_t**)b;

  // mutex_a points to a pointer to the first mutex.
  // If we follow it once, we get the pointer to the mutex.
  // This pointer is the memory address we want for comparison.
  return (*mutex_a < *mutex_b) ? -1 : ((*mutex_a == *mutex_b) ? 0 : 1);
}
