#define _GNU_SOURCE
#include "testlib.h"
#include "pipe.h"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <stdio.h>

int fd1 = 3;
int fd2 = 4;

int pipe(int pipefd[2]) {
    pipefd[0] = fd1;
    pipefd[1] = fd2;
    return 0;
}

int pipe2(int pipefd[2], int flags) {
    pipefd[0] = fd1;
    pipefd[1] = fd2;
    (void) flags;
    return 0;
}

pid_t fork(void) {
    return 42;
}

int close(int fd) {
    if (fd == fd1) {
        fd1 = 0;
    }
    if (fd == fd2) {
        fd2 = 0;
    }
    return 0;
}

int execvp(const char *file, char *const argv[]) {
    (void) file;
    (void) argv;
    return -1;
}

ssize_t write(int fd, const void *buf, size_t count) {
    (void) fd;
    (void) buf;
    return count;
}

ssize_t read(int fd, void *buf, size_t count) {
    (void) fd;
    (void) buf;
    return count;
}


int main() {
    test_start("You closed the pipe in the parent process.");
    test_plan(2);

    char *argv1[] = {
        "ls",
        ".",
        NULL
    };
    run_program("ls", argv1);

    test_equals_int(fd1, 0, "Read end closed");
    test_equals_int(fd2, 0, "Write end closed");
    return test_end();
}

