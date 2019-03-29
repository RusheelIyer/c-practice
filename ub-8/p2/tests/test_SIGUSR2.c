#define _POSIX_C_SOURCE 199309L
#include "testlib.h"
#include "priority_interrupts.h"
#include <signal.h>
#include <string.h>

static int state = 0;

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
    if (signum == SIGUSR2 && state == 0) {
        test_assert(1, "You register a SIGUSR2 signal handler");
        state++;
        if (act != NULL && act->sa_handler != SIG_DFL && act->sa_handler != SIG_IGN) {
            if (act->sa_flags & SA_SIGINFO) {
                siginfo_t info;
                memset(&info, 0, sizeof(siginfo_t));
                info.si_signo = SIGUSR2;
                act->sa_sigaction(SIGUSR2, &info, NULL);
            } else {
                act->sa_handler(SIGUSR2);
            }
            test_assert(1, "Your SIGUSR2 sigaction is correct");
            state++;
        }
    }
    if (oldact != NULL) {
        memset(oldact, 0, sizeof(struct sigaction));
        oldact->sa_handler = SIG_DFL;
    }
    return 0;
}

void sig() {
	test_equals_int(state, 1, "You registered the correct function for SIGUSR2");
}

void empty() {
}

void runnable() {
	test_equals_int(state, 2, "You registered the SIGUSR2 handler before calling runnable");
}

int main() {
	test_start("You register the right signal for SIGUSR2 before calling runnable().");
        test_plan(4);
	run_with_signals(runnable, empty, sig);

	return test_end();
}

