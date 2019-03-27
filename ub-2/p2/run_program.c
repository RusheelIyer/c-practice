#include "run_program.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ERROR_CODE 127

int run_program(char *file_path, char *arguments[]) {
  (void)file_path;
  (void)arguments;

  if ((file_path == NULL) || (arguments == NULL)) {
    return ERROR_CODE;
  }
  int child_pid;
  int status;
  
  child_pid = fork();
  if (child_pid == -1) {
    return ERROR_CODE;
  } else if (child_pid == 0) {
    int num = 0;
    char **narguments;

    // The first argument that we provide exec() should contain the
    // program that we want to execute. We therefore have to copy the
    // provided argument list and prepend it with file_path

    // First determine the length of arguments
    while (arguments[num++] != NULL)
      ;

    // Allocate memory for the new argument vector
    // The vector is one element longer
    narguments = (char **)malloc(sizeof(char *) * (num + 1));
    if (narguments == NULL) {
      exit(ERROR_CODE);
    }

    // Build the new argument vector by setting the first element to
    // file_path and copying the original argument vector
    narguments[0] = file_path;
    memcpy(&narguments[1], arguments, sizeof(char *) * num);

    // We are the child. Call exec
    execvp(narguments[0], narguments);

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
