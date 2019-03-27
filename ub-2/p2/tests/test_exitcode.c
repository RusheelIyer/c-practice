#include "testlib.h"
#include "run_program.h"

#include <sys/types.h>
#include <unistd.h>

int main() {
    test_start("You can start programs and return the exit code.");
    test_plan(2);

    char *argv1[] = {
        "--help",
        NULL
    };
    int result1 = run_program("gcc", argv1);
    test_equals_int(result1, 0, "gcc exits with code 0");

    char * argv2[] = {
        "-illegal-arg",
        NULL
    };
    int result2 = run_program("gcc", argv2);
    test_equals_int(result2, 1, "gcc -illegal-arg exits with code 1");

    return test_end();
}

