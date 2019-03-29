#include "testlib.h"
#include "message_queue.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <mqueue.h>

#define COMMAND_EXIT 0
#define COMMAND_ADD 1
#define COMMAND_SUBTRACT 2

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

int mq_send(mqd_t mqdes, const char *msg_ptr,
                     size_t msg_len, unsigned msg_prio) {
    test_equals_int(mqdes, 42, "mq_send: mqdes is correct");
    test_equals_int(msg_len, sizeof(Message), "mq_send: msg_len is correct");
    Message* cmd = (Message*) msg_ptr;
    test_assert(msg_len >= sizeof(Message) && cmd->command == CmdExit, "mq_send: command is correct");
    (void) msg_prio;
    return 0;
}


int main() {
    test_start("You called mq_send in sendExitTask correctly.");
    test_plan(4);

    test_equals_int(sendExitTask(42), 0, "sendExitTask returns 0");

    return test_end();
}

