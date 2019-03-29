#include "testlib.h"
#include "message_queue.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <mqueue.h>

#define QUEUE_NAME "/simple_calculator"

mqd_t mq_open(const char *name, int oflag, ...) {
    test_equals_string(name, QUEUE_NAME, "Queue is named " QUEUE_NAME);
    test_assert(oflag & O_WRONLY, "O_WRONLY flag is set");
    return 42;
}


int main() {
    test_start("You called mq_open in startClient().");
    test_plan(3);

    test_equals_int(startClient(), 42, "startClient() returns message queue id");

    return test_end();
}
