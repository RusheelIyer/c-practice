#include "testlib.h"
#include "run_program.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int __wrap_fork() {
    return 0;
}

int __wrap_execv(const char *path, char *const argv[]) {
    test_equals_string(path, "/bin/abcde", "You call exec with the correct path");
    test_equals_string(argv[0], "/bin/abcde", "You set argv[0] correctly");

    return 0;
}

int __wrap_execvp(const char *file, char *const argv[]) {
    return __wrap_execv(file, argv);
}


int __wrap_wait(int *stat_loc) {
    (void) stat_loc;
    return 2;
}

int __wrap_waitpid(pid_t pid, int *stat_loc, int options) {
    (void) pid;
    (void) stat_loc;
    (void) options;
    return 2;
}

int main() {
    test_start("You start the program using exec.");
    test_plan(2);

    char * argv[] = {
        NULL
    };
    run_program("/bin/abcde", argv);

    return test_end();
}
