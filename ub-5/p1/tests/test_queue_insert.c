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

int main() {
    test_start("Queue: You insert a new item correctly.");

    Queue q = {NULL, NULL};

    _enqueue(&q, 42);

    test_equals_ptr(q.tail, q.head, "With a single item, head equals tail");
    test_equals_ptr(q.head->next, NULL, "next is NULL");
    test_equals_int(q.head->data, 42, "data is saved correctly");

    return test_end();
}

