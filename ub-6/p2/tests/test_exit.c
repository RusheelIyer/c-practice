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
 * The commands supported by the server
 */
typedef enum _Command {
    CmdExit      = 0x00,     // Stops the server

    CmdAdd,                  // Adds the two message parameters
    CmdSubtract              // Subtracts the two message parameters
} Command;

/*
 * The message format to be sent to the server.
 */
typedef struct _Message {
    /*
     * One of the command constants.
     */
    Command command;
    /*
     * Used as operand 1 (if required)
     */
    int parameter1;
    /*
     * Used as operand 2 (if required)
     */
    int parameter2;
} Message;

#define QUEUE_NAME "/simple_calculator"

mqd_t our_mqdes;

ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio) {
    (void) msg_prio;
    our_mqdes = mqdes;
    test_equals_int(msg_len, sizeof(Message), "mq_receive: msg_len is correct");
    Message *cmd = (Message*) msg_ptr;
    cmd->command = CmdExit;
    return sizeof(Message);
}

int __real_mq_close(mqd_t mqdes);
int __wrap_mq_close(mqd_t mqdes) {
    test_equals_int(mqdes, our_mqdes, "mq_close: mqdes is correct");
    return __real_mq_close(mqdes);
}

int __real_mq_unlink(const char *name);
int __wrap_mq_unlink(const char *name) {
    test_equals_string(name, QUEUE_NAME, "mq_unlink: queue name is correct");
    return __real_mq_unlink(name);
}

int main() {
    test_start("Your server exits when receiving an exit command.");
    test_plan(4);
    __real_mq_unlink("/simple_calculator");

    test_equals_int(runServer(), 0, "The server exits with return value 0");
    __real_mq_unlink("/simple_calculator");

    return test_end();
}

