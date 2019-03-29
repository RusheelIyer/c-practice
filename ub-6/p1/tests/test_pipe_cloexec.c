#define _GNU_SOURCE
#include "testlib.h"
#include "pipe.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int __real_pipe2(int pipefd[2], int flags);
int __wrap_pipe2(int pipefd[2], int flags) {
    // This is the nice way to do it.
    if (flags & O_CLOEXEC) {
        test_assert(1, "You call pipe2() with O_CLOEXEC");
    }
    return __real_pipe2(pipefd, flags);
}

int __real_fcntl(int fd, int cmd, ...);
int __wrap_fcntl(int fd, int cmd, ...) {
    va_list argp;
    va_start(argp, cmd);
    if (cmd == F_SETFD && (va_arg(argp, int) & (FD_CLOEXEC))) {
        // This is the not-so-good way.
        test_assert(1, "You set FD_CLOEXEC via fcntl()");
    }
    va_end(argp);
    return __real_fcntl(fd, cmd, argp);
}


int main() {
    test_start("You set the O_CLOEXEC flag for the pipe.");
    test_plan(1);

    char *argv1[] = {
        "ls",
        ".",
        NULL
    };
    run_program("ls", argv1);

    return test_end();
}
