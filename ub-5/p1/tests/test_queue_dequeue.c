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

int main() {
    test_start("Queue: Your enqueue and dequeue works.");

    Queue q1 = {NULL, NULL};
    Queue q2 = {NULL, NULL};

    _enqueue(&q1, 42);
    _enqueue(&q1, 43);
    _enqueue(&q1, 44);
    _enqueue(&q1, 45);
    _enqueue(&q1, 46);

    test_equals_int(_dequeue(&q1), 42, "_dequeue -> 42");
    test_equals_int(_dequeue(&q1), 43, "_dequeue -> 43");
    test_equals_int(_dequeue(&q1), 44, "_dequeue -> 44");
    test_equals_int(_dequeue(&q1), 45, "_dequeue -> 45");
    test_equals_int(_dequeue(&q1), 46, "_dequeue -> 46");
    test_equals_int(_dequeue(&q1), -1, "_dequeue -> -1 (queue empty)");

    _enqueue(&q2, 42);
    test_equals_int(_dequeue(&q2), 42, "_dequeue -> 42");
    _enqueue(&q2, 43);
    test_equals_int(_dequeue(&q2), 43, "_dequeue -> 43");
    test_equals_int(_dequeue(&q2), -1, "_dequeue -> -1 (queue empty)");
    _enqueue(&q2, 44);
    _enqueue(&q2, 45);
    test_equals_int(_dequeue(&q2), 44, "_dequeue -> 44");
    test_equals_int(_dequeue(&q2), 45, "_dequeue -> 45");


    return test_end();
}

