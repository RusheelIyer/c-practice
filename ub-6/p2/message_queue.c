#include "message_queue.h"
#include <fcntl.h> /* For O_* constants */
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> /* For mode constants */

/*
 * The commands supported by the server
 */
typedef enum _Command
{
  CmdExit = 0x00, // Stops the server

  CmdAdd,     // Adds the two message parameters
  CmdSubtract // Subtracts the two message parameters
} Command;

/*
 * The message format to be sent to the server.
 */
typedef struct _Message
{
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

#define FORMAT_STRING_ADD "%d + %d = %d\n"
#define FORMAT_STRING_SUBTRACT "%d - %d = %d\n"

mqd_t
startClient(void)
{

    return mq_open(QUEUE_NAME, O_WRONLY);

}

int
sendExitTask(mqd_t client)
{
  Message sendMsg;
  sendMsg.command = CmdExit;

  return mq_send(client, (char*)&sendMsg, sizeof(Message), 0);
}

int
sendAddTask(mqd_t client, int operand1, int operand2)
{
  Message sendMsg;
  sendMsg.command = CmdAdd;
  sendMsg.parameter1 = operand1;
  sendMsg.parameter2 = operand2;

  return mq_send(client, (char*)&sendMsg, sizeof(Message), 0);
}

int
sendSubtractTask(mqd_t client, int operand1, int operand2)
{
  Message sendMsg;
  sendMsg.command = CmdSubtract;
  sendMsg.parameter1 = operand1;
  sendMsg.parameter2 = operand2;

  return mq_send(client, (char*)&sendMsg, sizeof(Message), 0);
}

int
stopClient(mqd_t client)
{
  return mq_close(client);
}

int
runServer(void)
{
  Message message;

  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = sizeof(Message);
  attr.mq_curmsgs = 0;

  mqd_t dest = mq_open(QUEUE_NAME,
                       O_RDONLY | O_CREAT | O_EXCL,
                       S_IWUSR | S_IWGRP | S_IWOTH,
                       &attr);

  if (dest == -1)
    return -1;

  do {
    ssize_t receive = mq_receive(dest, (char *) &message , sizeof(Message), NULL);

    if (receive == -1)
        return -1;

    switch(message.command) {
        case CmdAdd:
          printf(FORMAT_STRING_ADD,
                 message.parameter1,
                 message.parameter2,
                 message.parameter1 + message.parameter2);
          break;
        case CmdSubtract:
          printf(FORMAT_STRING_SUBTRACT,
                 message.parameter1,
                 message.parameter2,
                 message.parameter1 - message.parameter2);
          break;
        default:
            message.command = CmdExit;
    }

  } while (message.command != CmdExit);

  if (mq_close(dest) == -1) {
    return -1;
  }

  if (mq_unlink(QUEUE_NAME) == -1)
    return -1;

  return 0;
}
