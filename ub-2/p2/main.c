#include "testlib.h"
#include "run_program.h"
#include <stdlib.h>
#include <unistd.h>

int main()
{
    test_start("run_program.c");

    char *argv1[] = {
        "/",
        NULL
    };
    int result1 = run_program("/bin/ls", argv1);
    test_equals_int(result1, 0, "ls exits with code 0");

    char * argv2[] = {
        NULL
    };
    int result2 = run_program("/bin/false", argv2);
    test_equals_int(result2, 1, "false exits with code 1");

    return test_end();
}
