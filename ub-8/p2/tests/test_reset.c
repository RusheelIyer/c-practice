#define _POSIX_C_SOURCE 199309L
#include "testlib.h"
#include "priority_interrupts.h"
#include <signal.h>
#include <string.h>

int __real_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
int __wrap_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
    if (signum == SIGUSR2 && act != NULL && act->sa_handler == SIG_DFL) {
        test_assert(1, "You reset SIGUSR2");
    }
    if (signum == SIGUSR1 && act != NULL && act->sa_handler == SIG_DFL) {
        test_assert(1, "You reset SIGUSR1");
    }
    return __real_sigaction(signum, act, oldact);
}

void sig() {
}

void empty() {
}

void runnable() {
}

int main() {
    test_start("You set the default actions when runnable() is done.");
    test_plan(2);
    run_with_signals(runnable, empty, sig);

    return test_end();
}

