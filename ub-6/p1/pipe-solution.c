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

#define READ_END  0
#define WRITE_END 1

int run_program(char *file_path, char *argv[])
{
    int pipefd[2];

    if ((file_path == NULL) || (argv == NULL)) {
        return -1;
    }

    // We create the new pipe.
    // O_CLOEXEC is given so that the pipe is automatically closed when
    // execvp succeeds. This is necessary so we do not leak the pipe handle to
    // new exec'ed program. Setting the flag later would also work, but this
    // solution is cleaner (see man page for more information).
    if (pipe2(pipefd, O_CLOEXEC) == -1) {
        return -1;
    }

    // We can now use pipefd[READ_END] to read from the pipe and
    // pipefd[WRITE_END] to write to it.

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

        // This line is reached if execvp failed.
        // Let us send the resulting error number to the parent process.
        if (write(pipefd[WRITE_END], &errno, sizeof(errno)) < 0)
            exit(1);
        printf("execvp failure\n");

        // We exit normally.
        // The pipe is closed by exit.
        exit(0);
        printf("exit failed?\n");
    } else {
        int status, hadError = 0;

        // The parent does not need to write to the pipe.
        // Close it as soon as possible.
        close(pipefd[WRITE_END]);

        int waitError = waitpid(child_pid, &status, 0);
        if (waitError == -1) {
            // Error while waiting for child.
            hadError = 1;
        } else if (!WIFEXITED(status)) {
            // Our child exited with another problem (e.g., a segmentation fault)
            // We use the error code ECANCELED to signal this.
            hadError = 1;
            errno = ECANCELED;
        } else if (read(pipefd[READ_END], &errno, sizeof(errno)) > 0) {
            // The child sent us an error number on the pipe. We directly write
            // the error code to errno.
            hadError = 1;
        }

        // Clean up the leftover read end of the pipe. At this point errno is
        // set to the value we would like to present to the caller of this
        // function. Since the close call could change errno, we save and
        // restore it.
        const int oldError = errno;
        close(pipefd[READ_END]);
        errno = oldError;

        return hadError ? -1 : WEXITSTATUS(status);
    }
}
