#ifndef BARRIER_H
#define BARRIER_H

typedef struct _ThreadBarrier ThreadBarrier;

ThreadBarrier *createBarrier(int threads);

void enterBarrier(ThreadBarrier *barrier);

void deleteBarrier(ThreadBarrier *barrier);

#endif

