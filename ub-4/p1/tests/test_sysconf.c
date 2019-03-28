#include "testlib.h"
#include "workerpool.h"

#include <string.h>
#include <unistd.h>

// Wrapped by compiling with -Wl,--wrap=sysconf
long __wrap_sysconf(int name) {
    test_equals_int(name, _SC_NPROCESSORS_ONLN, "You call sysconf(_SC_NPROCESSORS_ONLN)");
    return -1;
}

int main() {
    test_start("You use sysconf to get the number of processors and do proper error checking.");
    test_plan(2);

    test_equals_int(initializeWorkerPool(), -1, "initializeWorkerPool fails");

    return test_end();
}
