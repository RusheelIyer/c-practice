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

pid_t fork(void) {
    return 0;
}

int execvp(const char *file, char *const argv[]) {
    (void) file;
    (void) argv;
    errno = 78;
    return -1;
}

ssize_t write(int fd, const void *buf, size_t count) {
    (void) fd;
    for (size_t i = 0; i < count; i++) {
        if (((char*)buf)[i] == 78) {
            test_assert(1, "You write the correct value to the pipe");
        }
    }
    return count;
}

int main() {
    test_start("You write the error number to the pipe.");
    test_plan(1);

    char *argv1[] = {
        "ls",
        ".",
        NULL
    };
    run_program("ls", argv1);

    return test_end();
}

