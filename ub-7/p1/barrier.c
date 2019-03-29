#include "barrier.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lock;

struct _ThreadBarrier {
  pthread_cond_t condition;
  unsigned int threads;
  unsigned int curThreads;
};

ThreadBarrier* createBarrier(int threads) {
  ThreadBarrier* barrier = malloc(sizeof(ThreadBarrier));

  if (barrier == NULL || threads < 2) {
    return NULL;
  }

  barrier->threads = threads;
  barrier->curThreads = 0;
  pthread_cond_init(&barrier->condition, NULL);

  return barrier;
}

void enterBarrier(ThreadBarrier* barrier) {
  barrier->curThreads++;

  if (barrier->curThreads < barrier->threads) {
    pthread_cond_wait(&barrier->condition, &lock);
  } else {
    barrier->curThreads = 0;
    pthread_cond_broadcast(&barrier->condition);
  }
}

void deleteBarrier(ThreadBarrier* barrier) {
  free(barrier);
}
