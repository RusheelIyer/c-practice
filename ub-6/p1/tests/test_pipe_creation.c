#include "testlib.h"
#include "pipe.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int pipe(int pipefd[2]) {
    (void) pipefd;
    test_assert(1, "You call pipe()");
    return -1;
}

int pipe2(int pipefd[2], int flags) {
    (void) pipefd;
    (void) flags;
    test_assert(1, "You call pipe2()");
    return -1;
}

int main() {
    test_start("You created the pipe.");
    test_plan(1);

    char *argv1[] = {
        "ls",
        ".",
        NULL
    };
    run_program("ls", argv1);

    return test_end();
}
