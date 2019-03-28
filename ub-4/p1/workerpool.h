#ifndef WORKERPOOL_H
#define WORKERPOOL_H

/*
 * Maximum number of outstanding jobs
 */
#define MAX_JOBS 10

typedef void (*WorkFunc)(int arg);

typedef struct _WorkItem {
    /*
     * The function that the worker should execute for this item.
     */
    WorkFunc func;
    /*
     * The argument for the job.
     */
    int arg;
} WorkItem;

int initializeWorkerPool(void);
void finalizeWorkerPool(void);

int getWorkerId(void);

int submitWork(WorkFunc func, int arg);

#endif

