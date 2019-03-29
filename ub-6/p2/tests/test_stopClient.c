#include "testlib.h"
#include "message_queue.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <mqueue.h>

int mq_close(mqd_t mqdes) {
    test_equals_int(mqdes, 42, "mq_close: mqdes is correct");
    return 0;
}

int main() {
    test_start("You called mq_close in stopClient correctly.");
    test_plan(2);

    test_equals_int(stopClient(42), 0, "stopClient returns 0");

    return test_end();
}

