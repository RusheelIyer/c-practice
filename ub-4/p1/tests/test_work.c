#define _XOPEN_SOURCE 600
#include "testlib.h"
#include "workerpool.h"

#include <string.h>
#include <unistd.h>

static void job(int arg) {
    test_equals_int(arg, 0xdeadbeef, "job argument is correct");
}

int main() {
    test_start("Your worker threads execute submitted work.");
    test_plan(2);

    test_equals_int(initializeWorkerPool(), 0, "initializeWorkerPool succeeds");

    submitWork(job, 0xdeadbeef);

    usleep(500);

    return test_end();
}
