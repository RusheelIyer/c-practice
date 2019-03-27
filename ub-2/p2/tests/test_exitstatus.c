#include "testlib.h"
#include "run_program.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int __wrap_fork() {
    return 42;
}

int __wrap_wait(int *stat_loc) {
    *stat_loc = 5 << 8;
    return 42;
}

int __wrap_waitpid(pid_t pid, int *stat_loc, int options) {
    (void) pid;
    (void) options;
    *stat_loc = 5 << 8;
    return 42;
}

// You compute the right exit status
int main() {
    test_start("run_program.c");
    test_plan(1);
    char * argv[] = {
        NULL
    };
    int status = run_program("/bin/abcde", argv);
    test_equals_int(status, 5, "exit status is correct");

    return test_end();
}
