#define _POSIX_SOURCE
#include "priority_interrupts.h"
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

int run_with_signals(void (*runnable)(void), void (*usr1)(void),
    void (*usr2)(void))
{
    if ((runnable == NULL) || (usr1 == NULL) || (usr2 == NULL)) {
        return -1;
    }

    // Register SIGUSR1 handler.
    // The handler function gets passed an int, but thanks to C calling
    // convention, this int is ignored by the function.
    // We store the current configuration of the signal in oldact1, so we can
    // restore it later (and on error).
    struct sigaction oldact1, act1 = {
        .sa_handler = (void(*)(int)) usr1,
        .sa_flags   = 0
    };

    sigemptyset(&act1.sa_mask);

    if (sigaction(SIGUSR1, &act1, &oldact1) != 0) {
        return -1;
    }

    // Register SIGUSR2 handler.
    // Signal SIGUSR1 is disabled while running the signal handler for SIGUSR2.
    struct sigaction oldact2, act2 = {
        .sa_handler = (void(*)(int)) usr2,
        .sa_flags   = 0
    };

    sigemptyset(&act2.sa_mask);
    sigaddset(&act2.sa_mask, SIGUSR1);

    if (sigaction(SIGUSR2, &act2, &oldact2) != 0) {
        sigaction(SIGUSR1, &oldact1, NULL); // Reset the first action.
        return -1;
    }

    // Now run the function with SIGUSR1 and SIGUSR2 enabled.
    runnable();

    // Restore the original signal handler configuration.
    sigaction(SIGUSR1, &oldact1, NULL);
    sigaction(SIGUSR2, &oldact2, NULL);

    return 0;
}

int set_irql_0(void)
{
    // The default level.
    // This allows all signal handlers.
    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    sigaddset(&signals, SIGUSR2);
    return sigprocmask(SIG_UNBLOCK, &signals, NULL);
}

int set_irql_1(void)
{
    // This allows only the SIGUSR2 interrupt handler.
    // Depending on the previous level, we either need to disable or enable
    // the right handlers.
    sigset_t oldsignals, signals;

    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &signals, &oldsignals) != 0) {
        return -1;
    }

    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR2);
    if (sigprocmask(SIG_UNBLOCK, &signals, NULL) != 0) {
        // Something went wrong. Restore the original state.
        sigprocmask(SIG_SETMASK, &oldsignals, NULL);
        return -1;
    }

    return 0;
}

int set_irql_2(void)
{
    // We lock at the highest level.
    // This means, that both SIGUSR1 and SIGUSR2 are disabled
    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    sigaddset(&signals, SIGUSR2);
    return sigprocmask(SIG_BLOCK, &signals, NULL);
}
