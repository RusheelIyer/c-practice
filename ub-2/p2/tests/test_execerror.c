#include "testlib.h"
#include "run_program.h"

#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    test_start("You return 127 on exec error.");
    test_plan(1);

    char *argv1[] = {
        NULL
    };
    int result1 = run_program("nonexistant-program", argv1);
    test_equals_int(result1, 127, "Starting nonexistant-program returns 127");

    return test_end();
}

