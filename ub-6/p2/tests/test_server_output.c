#define _GNU_SOURCE
#include "testlib.h"
#include "message_queue.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    test_start("Your server gives the right output.");
    mq_unlink("/simple_calculator");
    int filedes[2];
    if (pipe(filedes) == -1) {
        perror("pipe");
        test_failed_message("pipe() failed");
    }
    fcntl(filedes[1], F_SETPIPE_SZ, 4096 * 16);

    switch(fork()) {
    case -1:
        perror("fork");
        test_failed_message("fork() failed");
    case 0:
        close(filedes[0]);
        dup2(filedes[1], STDOUT_FILENO);
        close(filedes[1]);
        // Child is server
        printf("Starting test server\n");
        if (runServer() < 0) {
            perror("runServer");
            exit(1);
        }
        printf("Test server done\n");
        exit(0);
        break;
    default:
        close(filedes[1]);
        // Hope that the server is started now.
        usleep(100000);
        // Send some test commands to server.
        printf("Running test client\n");
        mqd_t clientHandle = startClient();
        if (clientHandle < 0) {
            perror("startClient");
            exit(1);
        }
        if (sendAddTask(clientHandle, 10, -2) < 0) {
            perror("sendAddTask");
        }
        if (sendSubtractTask(clientHandle, 5, 13) < 0) {
            perror("sendAddTask");
        }
        if (sendExitTask(clientHandle) < 0) {
            perror("sendExitTask");
        }
        if (stopClient(clientHandle) < 0) {
            perror("sendExitTask");
        }
        printf("Test client done\n");
        wait(NULL);

        char out[4096 * 16];
        int count = read(filedes[0], out, sizeof(out));
        if (count < 0) {
            perror("read");
            test_failed_message("read failed");
        }
        printf("Server output:\n");
        if (write(STDOUT_FILENO, out, count) < 0) {
            perror("write");
        }

        const char* expected = "10 + -2 = 8\n5 - 13 = -8\n";
        test_contains_string(out, expected, "Server output is correct");
    }

    return test_end();
}

