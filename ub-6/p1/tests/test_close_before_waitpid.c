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
    if (fd == fd2) {
        fd2 = 0;
        test_assert(1, "Write end closed");
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

pid_t waitpid(pid_t pid, int *status, int options) {
    (void) pid;
    (void) status;
    (void) options;
    test_equals_int(fd2, 0, "waitpid: write end is closed");
    return 0;
}


int main() {
    test_start("You closed the write end in the parent before calling waitpid.");
    test_plan(2);

    char *argv1[] = {
        "ls",
        ".",
        NULL
    };
    run_program("ls", argv1);

    return test_end();
}

