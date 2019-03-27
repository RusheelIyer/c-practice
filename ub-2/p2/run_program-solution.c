#include "run_program.h"
#include <sys/wait.h> /* For waitpid */
#include <unistd.h> /* For fork */
#include <stdlib.h> /* For exit */
#include <string.h> /* For memcpy */

#define ERROR_CODE 127

int run_program(char *file_path, char *argv[])
{
    int status;
    int child_pid;

    if ((file_path == NULL) || (argv == NULL)) {
        return ERROR_CODE;
    }

    child_pid = fork();
    if (child_pid == -1) {
        return ERROR_CODE;
    } else if (child_pid == 0) {
        // This code is executed in the child -----
        int num = 0;
        char **nargv;

        // The first argument that we provide exec() should contain the
        // program that we want to execute. We therefore have to copy the
        // provided argument list and prepend it with file_path

        // First determine the length of argv
        while (argv[num++] != NULL);

        // Allocate memory for the new argument vector
        // The vector is one element longer
        nargv = (char**)malloc(sizeof(char*) * (num + 1));
        if (nargv == NULL) {
            exit(ERROR_CODE);
        }

        // Build the new argument vector by setting the first element to
        // file_path and copying the original argument vector
        nargv[0] = file_path;
        memcpy(&nargv[1], argv, sizeof(char*) * num);

        // We are the child. Call exec
        execvp(nargv[0], nargv);

        // This line is reached if exec failed. Otherwise, the child continues
        // execution in the main of the new program.
        exit(ERROR_CODE);
    } else {
        // This code is executed in the parent -----
        if (waitpid(child_pid, &status, 0) == -1) {
            return ERROR_CODE;
        }

        return (WIFEXITED(status)) ? WEXITSTATUS(status) : ERROR_CODE;
    }
}
