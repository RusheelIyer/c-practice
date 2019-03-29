#include "testlib.h"
#include "message_queue.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <mqueue.h>
#include <stdarg.h>

/*
 * The message format to be sent to the server.
 */
typedef struct {
    /*
     * One of the command constants.
     */
    int command;
    /*
     * Operand 1 (if required)
     */
    int operand1;
    /*
     * Operand 2 (if required)
     */
    int operand2;
} CalculatorCommand;

#define QUEUE_NAME "/simple_calculator"

mqd_t mq_open(const char *name, int oflag, ...) {
    test_equals_string(name, QUEUE_NAME, "mq_open: name is correct");
    // O_RDONLY is 0 :-(
    // test_assert(oflag & O_RDONLY, "mq_open: O_RDONLY is set");
    test_assert(oflag & O_CREAT, "mq_open: O_CREAT flag is set");
    if (oflag & O_CREAT) {
        va_list argp;
        va_start(argp, oflag);
        int i = va_arg(argp, int);
        struct mq_attr *attr = va_arg(argp, struct mq_attr *);

        test_assert((i & 0200) == 0200, "mq_open: S_IWUSR mode is set");
        test_equals_int(attr->mq_msgsize, sizeof(CalculatorCommand), "mq_open: mq_msgsize is correct");
        va_end(argp);
    }

    return -1;
}


int main() {
    test_start("You call mq_open in runServer correctly.");
    test_plan(5);

    test_equals_int(runServer(), -1, "runServer returns -1");

    return test_end();
}

