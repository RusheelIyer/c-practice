#define _POSIX_C_SOURCE 199309L
#include "testlib.h"
#include "priority_interrupts.h"
#include <signal.h>
#include <string.h>

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
    if (signum == SIGUSR2 && act != NULL && act->sa_handler != SIG_DFL && act->sa_handler != SIG_IGN) {
        test_assert(sigismember(&act->sa_mask, SIGUSR1), "You mask out SIGUSR1");
    }
    if (oldact != NULL) {
        memset(oldact, 0, sizeof(struct sigaction));
        oldact->sa_handler = SIG_DFL;
    }
    return 0;
}
void sig() {
}

void empty() {
}

void runnable() {
}

int main() {
    test_start("You mask out SIGUSR1 when the signal handler for SIGUSR2 gets called.");
    test_plan(1);
    run_with_signals(runnable, empty, sig);

    return test_end();
}

