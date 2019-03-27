#include "testlib.h"
#include "run_program.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int __wrap_fork() {
    return 42;
}

int __wrap_wait(int *stat_loc) {
    (void) stat_loc;
    test_assert(1, "You call wait()");
    return 42;
}

int __wrap_waitpid(pid_t pid, int *stat_loc, int options) {
    (void) stat_loc;
    (void) options;
    test_equals_int(pid, 42, "You call waitpid() with the correct pid");
    return 42;
}

int main() {
    test_start("You wait for the process to exit.");
    test_plan(1);

    char * argv[] = {
        NULL
    };
    run_program("/bin/abcde", argv);

    return test_end();
}
