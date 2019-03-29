#define _POSIX_C_SOURCE 199309L
#include "testlib.h"
#include "priority_interrupts.h"
#include <signal.h>
#include <stdlib.h>

int main() {
	test_start("Your interrupts_level_2() sets the right mask.");

	sigset_t signals;
	for (int i = 0; i < 2; i++) {
		sigemptyset(&signals);
		if (i > 0) {
			sigaddset(&signals, SIGUSR1);
		}
		if (i > 1) {
			sigaddset(&signals, SIGUSR2);
		}
		sigprocmask(SIG_SETMASK, &signals, NULL);

		set_irql_2();

		sigprocmask(SIG_SETMASK, NULL, &signals);

		test_equals_int(sigismember(&signals, SIGUSR1), 1, "SIGUSR1 is blocked");
		test_equals_int(sigismember(&signals, SIGUSR2), 1, "SIGUSR2 is blocked");
		test_equals_int(sigismember(&signals, SIGHUP),  0, "SIGHUP is not blocked");
		test_equals_int(sigismember(&signals, SIGINT),  0, "SIGINT is not blocked");
		test_equals_int(sigismember(&signals, SIGQUIT), 0, "SIGQUIT is not blocked");
		test_equals_int(sigismember(&signals, SIGILL),  0, "SIGILL is not blocked");
		test_equals_int(sigismember(&signals, SIGABRT), 0, "SIGABRT is not blocked");
		test_equals_int(sigismember(&signals, SIGFPE),  0, "SIGFPE is not blocked");
		test_equals_int(sigismember(&signals, SIGKILL), 0, "SIGKILL is not blocked");
		test_equals_int(sigismember(&signals, SIGSEGV), 0, "SIGSEGV is not blocked");
		test_equals_int(sigismember(&signals, SIGPIPE), 0, "SIGPIPE is not blocked");
		test_equals_int(sigismember(&signals, SIGALRM), 0, "SIGALRM is not blocked");
		test_equals_int(sigismember(&signals, SIGTERM), 0, "SIGTERM is not blocked");
		test_equals_int(sigismember(&signals, SIGCHLD), 0, "SIGCHLD is not blocked");
		test_equals_int(sigismember(&signals, SIGCONT), 0, "SIGCONT is not blocked");
		test_equals_int(sigismember(&signals, SIGSTOP), 0, "SIGSTOP is not blocked");
	}

	return test_end();
}

