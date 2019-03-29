// This changes the way some includes behave.
// This should stay before any include.
#define _GNU_SOURCE

#include "pipe.h"
#include <sys/wait.h> /* For waitpid */
#include <unistd.h> /* For fork, pipe */
#include <stdlib.h> /* For exit */
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#define READ_END 0
#define WRITE_END 1

int run_program(char *file_path, char *argv[])
{

    if ((file_path == NULL) || (argv == NULL)) {
        return -1;
    }

    // Open a pipe
    int pipefd[2];
    int pipeStatus = pipe2(pipefd, O_CLOEXEC);

    if (pipeStatus == -1) {
        return pipeStatus;
    }

    int child_pid = fork();
    if (child_pid == -1) {
        close(pipefd[READ_END]);
        close(pipefd[WRITE_END]);

        return -1;
    } else if (child_pid == 0) {

        // The child process won't need to read from the pipe.
        // It is good practice to close the unneeded end of the pipe
        // as soon as possible.
        close(pipefd[READ_END]);

        // Replace program
        execvp(file_path, argv);

        int writeStatus = (int) write(pipefd[WRITE_END], &errno, sizeof(errno));

        if (writeStatus < 0)
            exit(1);
        printf("execvp failure\n");

        exit(0);
        printf("exit failed?\n");

    } else {
        int status, hadError = 0;

        int waitError = waitpid(child_pid, &status, 0);
        if (waitError == -1) {
            // Error while waiting for child.
            hadError = 1;
        } else if (!WIFEXITED(status)) {
            // Our child exited with another problem (e.g., a segmentation fault)
            // We use the error code ECANCELED to signal this.
            hadError = 1;
            errno = ECANCELED;
        } else {
            int readStatus = (int) read(pipefd[READ_END], &errno, sizeof(errno));

            if (readStatus < 0)
                hadError = 1;
        }

        return hadError ? -1 : WEXITSTATUS(status);
    }
}
