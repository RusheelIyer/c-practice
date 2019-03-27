#include "testlib.h"
#include "run_program.h"

#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

static int fork_result = 42, wait_result = -1;

int __wrap_fork() {
    return fork_result;
}

int __wrap_wait(int *stat_loc) {
    *stat_loc = 0;
    errno = EACCES;
    return wait_result;
}

int __wrap_waitpid(pid_t pid, int *stat_loc, int options) {
    (void) pid;
    (void) stat_loc;
    (void) options;
    *stat_loc = 0;
    errno = EACCES;
    return wait_result;
}

int main() {
    test_start("You return 127 on fork/wait error.");
    test_plan(2);

    char *argv1[] = {
        NULL
    };
    int result1 = run_program("true", argv1);
    test_equals_int(result1, 127, "Failing wait() returns 127");

    // Now make fork() fail, but not wait().
    fork_result = -1;
    wait_result = 42;
    int result2 = run_program("true", argv1);
    test_equals_int(result2, 127, "Failing fork() returns 127");

    return test_end();
}

