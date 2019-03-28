#include "testlib.h"
#include "scheduler.h"
#include <stdlib.h>

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

typedef struct {
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

void _enqueue(Queue *queue, int data);

int _dequeue(Queue *queue);

// Note: This test relies on LeakSanitizer to detect memory leaks. See
// Makefile.
int main() {
    test_start("Queue: You have no memory leaks.");
    test_plan(2);

    Queue q1 = {NULL, NULL};

    _enqueue(&q1, 42);
    _enqueue(&q1, 43);
    test_equals_int(_dequeue(&q1), 42, "_dequeue -> 42");
    test_equals_int(_dequeue(&q1), 43, "_dequeue -> 43");

    return test_end();
}
