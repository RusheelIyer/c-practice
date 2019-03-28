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

/*
 * How often can a thread of the same priority be scheduled without needing
 * to schedule one of a lower priority.
 */
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
    assert(queue != NULL);

    QueueItem *item = (QueueItem*)malloc(sizeof(QueueItem));
    if (item == NULL) {
        return;
    }

    item->data = data;
    item->next = NULL;
    if (queue->head == NULL) {
        queue->head = item;
    } else {
        assert(queue->tail != NULL);

        queue->tail->next = item;
    }

    queue->tail = item;
}

/*
 * Remove and get the head of the queue.
 * Return -1 if the queue is empty.
 */
int _dequeue(Queue *queue)
{
    assert(queue != NULL);

    if (queue->head == NULL) {
        return -1;
    }

    QueueItem *head = queue->head;
    int data = head->data;
    queue->head = head->next;
    free(head);

    return data;
}

void initScheduler()
{

}

static void _enqueueThread(int threadId)
{
    assert((threadId >= 0) && (threadId < MAX_THREADS));
    assert(_threads[threadId].state == STATE_READY);

    // Append a thread to the right ready queue.
    Queue *queue = &_queues[_threads[threadId].priority];
    _enqueue(queue, threadId);
}

/*
 * Called whenever a waiting thread gets ready to run.
 */
void onThreadReady(int threadId)
{
    assert((threadId >= 0) && (threadId < MAX_THREADS));
    assert(_threads[threadId].state == STATE_WAITING);

    _threads[threadId].state = STATE_READY;
    _enqueueThread(threadId);
}

/*
 * Called whenever a running thread is forced of the CPU
 * (e.g., through a timer interrupt).
 */
void onThreadPreempted(int threadId)
{
    assert((threadId >= 0) && (threadId < MAX_THREADS));
    assert(_threads[threadId].state == STATE_RUNNING);

    _threads[threadId].state = STATE_READY;
    _enqueueThread(threadId);
}

/*
 * Called whenever a running thread needs to wait.
 */
void onThreadWaiting(int threadId)
{
    assert((threadId >= 0) && (threadId < MAX_THREADS));
    assert(_threads[threadId].state == STATE_RUNNING);

    _threads[threadId].state = STATE_WAITING;
    // We do not need to enqueue waiting threads.
}

static int _dequeueThread(int priority)
{
    assert(priority <= HIGHEST_PRIORITY);
    if (priority < 0) {
        // Idle thread if priority is less than 0. This will abort recursion.
        return -1;
    }

    // Try to schedule a thread with lower priority after
    // MAX_SEQUENCE_LENGTH threads of this priority were scheduled.
    if (_currentSequence[priority] >= MAX_SEQUENCE_LENGTH) {
        int nextThread = _dequeueThread(priority - 1);

        // Reset the current sequence counter on any chance the lower
        // priority threads have, no matter if one is really running.
        _currentSequence[priority] = 0;
        if (nextThread != -1) {
            // Only send the replacement thread if it was not the idle thread.
            return nextThread;
        }
    }

    Queue *queue = &_queues[priority];
    int dequeued = _dequeue(queue);

    if (dequeued < 0) {
        // No more threads in this queue. We give a chance to threads with
        // lower priority, so we need to reset the sequence counter.
        _currentSequence[priority] = 0;
        return _dequeueThread(priority - 1);
    } else {
        // Schedule a thread with the current priority.
        _currentSequence[priority]++;
        return dequeued;
    }
}

/*
 * Gets the id of the next thread to run and sets its state to running.
 */
int scheduleNextThread()
{
    // Try to schedule a thread with the highest priority.
    // This will return a thread with a lower priority if required to prevent
    // starvation.
    int threadId = _dequeueThread(HIGHEST_PRIORITY);
    if (threadId != -1) {
        assert(threadId < MAX_THREADS);
        assert(_threads[threadId].state == STATE_READY);

        _threads[threadId].state = STATE_RUNNING;
    }

    return threadId;
}
