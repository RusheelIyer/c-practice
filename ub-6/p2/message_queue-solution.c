#include "message_queue.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <stdlib.h>
#include <stdio.h>

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

#define FORMAT_STRING_ADD      "%d + %d = %d\n"
#define FORMAT_STRING_SUBTRACT "%d - %d = %d\n"

mqd_t startClient(void)
{
    // Open the queue.
    // The server should already be running, so the queue does not need to be
    // created. The client only writes to the queue.
    return mq_open(QUEUE_NAME, O_WRONLY);
}

int sendExitTask(mqd_t client)
{
    Message msg;
    msg.command    = CmdExit;
    msg.parameter1 = 0;  // Not used
    msg.parameter2 = 0;  // Not used
    return mq_send(client, (char*)&msg, sizeof(msg), 0);
}

int sendAddTask(mqd_t client, int operand1, int operand2)
{
    Message msg;
    msg.command    = CmdAdd;
    msg.parameter1 = operand1;
    msg.parameter2 = operand2;
    return mq_send(client, (char*)&msg, sizeof(msg), 0);
}

int sendSubtractTask(mqd_t client, int operand1, int operand2)
{
    Message msg;
    msg.command    = CmdSubtract;
    msg.parameter1 = operand1;
    msg.parameter2 = operand2;
    return mq_send(client, (char*)&msg, sizeof(msg), 0);
}

int stopClient(mqd_t client)
{
    return mq_close(client);
}

int runServer(void)
{
    int didExit = 0, hadError = 0;
    Message msg;

    struct mq_attr attr;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = 10;           // Maximum number of messages in the queue
    attr.mq_msgsize = sizeof(msg);  // Maximum message size
    attr.mq_curmsgs = 0;

    // Create and open the message queue. Server only needs to read from it.
    // Clients only need to write to it, allow for all users.
    mqd_t server = mq_open(QUEUE_NAME,
                           O_RDONLY | O_CREAT | O_EXCL,
                           // This is equal to 0222
                           S_IWUSR | S_IWGRP | S_IWOTH,
                           &attr);
    if (server == -1) {
        return -1;
    }

    do {
        // Attempt to receive a message from the queue.
        ssize_t received = mq_receive(server, (char*)&msg, sizeof(msg), NULL);
        if (received != sizeof(msg)) {
            // This implicitly also checks for error (i.e., -1)
            hadError = 1;
            continue;
        }

        switch (msg.command)
        {
            case CmdExit:
                // End this loop.
                didExit = 1;
                break;

            case CmdAdd:
                // Print the required output.
                printf(FORMAT_STRING_ADD,
                       msg.parameter1,
                       msg.parameter2,
                       msg.parameter1 + msg.parameter2);
                break;

            case CmdSubtract:
                // Print the required output.
                printf(FORMAT_STRING_SUBTRACT,
                       msg.parameter1,
                       msg.parameter2,
                       msg.parameter1 - msg.parameter2);
                break;

            default:
                hadError = 1;
                break;
        }

    } while (!didExit && !hadError);

    // Always attempt to close the server queue.
    if (mq_close(server) == -1) {
        hadError = 1;
    }

    // The server queue needs to be unlinked.
    if (mq_unlink(QUEUE_NAME) == -1) {
        hadError = 1;
    }

    return hadError ? -1 : 0;
}