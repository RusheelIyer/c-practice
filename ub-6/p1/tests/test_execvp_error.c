#include "testlib.h"
#include "pipe.h"
#include <errno.h>
#include <stdlib.h>

int main() {
    test_start("You return -1 on execvp errors.");
    test_plan(5);

    errno = 0;
    char * argv1[] = {
        "ls",
        ".",
        NULL
    };
    int result = run_program("ls", argv1);
    test_equals_int(result, 0, "ls exits with result 0");

    errno = 0;
    char * argv2[] = {
        "/djals/sahda/unknown-program-name",
        NULL
    };
    int result2 = run_program("/djals/sahda/unknown-program-name", argv2);
    test_equals_int(result2, -1, "unknown-program-name fails");
    test_equals_int(errno, ENOENT, "errno is ENOENT");

    errno = 0;
    char * argv3[] = {
        "/bin",
        NULL
    };
    int result3 = run_program("/bin", argv3);
    test_equals_int(result3, -1, "/bin fails");
    test_equals_int(errno, EACCES, "errno is EACCESS");

    return test_end();
}

