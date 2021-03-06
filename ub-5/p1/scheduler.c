#include "scheduler.h"
#include <stdlib.h>
#include <assert.h>

typedef struct _QueueItem {
    /*
     * The data of this item.
     */
    int data;
    /*
     * The next item in the queue.
     * NULL if there is no next item.
     */
    struct _QueueItem *next;
} QueueItem;

typedef struct _Queue {
    /*
     * The first item of the queue.
     * NULL if the queue is empty.
     */
    QueueItem *head;
    /*
     * The last item of the queue.
     * undefined if the queue is empty.
     */
    QueueItem *tail;
} Queue;

typedef enum _ThreadState {
    STATE_UNUSED = 0, // This entry in the _threads array is unused.

    STATE_READY,      // The thread is ready and should be on a ready queue for
                      //   selection by the scheduler
    STATE_RUNNING,    // The thread is running and should not be on a ready queue
    STATE_WAITING     // The thread is blocked and should not be on a ready queue
} ThreadState;

typedef struct _Thread {
    int threadId;
    ThreadState state;
    /*
     * Range: 0 ... HIGHEST_PRIORITY (including)
     * HIGHEST_PRIORITY is highest priority
     */
    int priority;
} Thread;

Thread _threads[MAX_THREADS] = {{0}};

#define MAX_SEQUENCE_LENGTH 5

/*
 * The ready queues for each priority.
 */
Queue _queues[HIGHEST_PRIORITY + 1] = {{0}};

/*
 * Counter to determine after how many scheduling decisions a thread of a lower
 * priority needs to be chosen.
 */
int _currentSequence[HIGHEST_PRIORITY + 1] = {0};

/*
 * Adds a new, waiting thread.
 */
int startThread(int threadId, int priority)
{
    if (((threadId < 0) || (threadId >= MAX_THREADS) ||
        (_threads[threadId].state != STATE_UNUSED)) ||
        (priority < 0) || (priority > HIGHEST_PRIORITY)) {

        return -1;
    }

    _threads[threadId].threadId = threadId;
    _threads[threadId].state    = STATE_WAITING;
    _threads[threadId].priority = priority;
    return 0;
}

/*
 * Append to the tail of the queue.
 * Does nothing on error.
 */
void _enqueue(Queue *queue, int data)
{
    QueueItem* item = (QueueItem*) malloc(sizeof(QueueItem));

    if (item != NULL) {
        item->data = data;
        item->next = NULL;
        if (queue->head == NULL) {
            queue->head = item;
        } else {
            queue->tail->next = item;
        }
        queue->tail = item;
    }
}

/*
 * Remove and get the head of the queue.
 * Return -1 if the queue is empty.
 */
int _dequeue(Queue *queue)
{
    QueueItem* curHead = queue->head;
    if (curHead == NULL)
        return -1;

    queue->head = curHead->next;
    int retData = curHead->data;
    free(curHead);

    return retData;

}

void initScheduler()
{
    // TODO: Implement if you need to initialize any global variables you added
}

/*
 * Called whenever a waiting thread gets ready to run.
 */
void onThreadReady(int threadId)
{
    _threads[threadId].state = STATE_READY;
    Queue *queue = &_queues[_threads[threadId].priority];
    _enqueue(queue, threadId);
}

/*
 * Called whenever a running thread is forced of the CPU
 * (e.g., through a timer interrupt).
 */
void onThreadPreempted(int threadId)
{
    _threads[threadId].state = STATE_READY;
    Queue *queue = &_queues[_threads[threadId].priority];
    _enqueue(queue, threadId);
}

/*
 * Called whenever a running thread needs to wait.
 */
void onThreadWaiting(int threadId)
{
    _threads[threadId].state = STATE_WAITING;
}

/*
 * Gets the id of the next thread to run and sets its state to running.
 */
int scheduleNextThread()
{
    Queue* queue = NULL;

    for (int i = HIGHEST_PRIORITY; i >= 0; i--) {
        if (_queues[i].head != NULL) {
            if (_currentSequence[i] >= MAX_SEQUENCE_LENGTH) {
                _currentSequence[i] = 0;
            } else {
                queue = &_queues[i];
                _currentSequence[i]++;
                //break the loop
                i = -1;
            }
        }
    }

    if (queue == NULL) {
        return -1;
    }

    int threadId = _dequeue(queue);
    _threads[threadId].state = STATE_RUNNING;
    return threadId;
}
