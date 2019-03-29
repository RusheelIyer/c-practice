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
    // TODO: Open the message queue previously created by the server
    return -1;
}

int sendExitTask(mqd_t client)
{
    (void)client;

    // TODO: Send the exit command to the server.
    return -1;
}

int sendAddTask(mqd_t client, int operand1, int operand2)
{
    (void)client;
    (void)operand1;
    (void)operand2;

    // TODO: Send the add command with the operands
    return -1;
}

int sendSubtractTask(mqd_t client, int operand1, int operand2)
{
    (void)client;
    (void)operand1;
    (void)operand2;

    // TODO: Send the sub command with the operands
    return -1;
}

int stopClient(mqd_t client)
{
    (void)client;

    // TODO: Clean up anything on the client-side
    return -1;
}

int runServer(void)
{
    // TODO: Implement the server
    return -1;
}