#define _POSIX_C_SOURCE 199309L
#include "testlib.h"
#include "priority_interrupts.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// This makes using nanosleep a bit more convenient.
static inline void usleep(unsigned int time)
{
    struct timespec waitTime = {time / 1000000, (time % 1000000) * 1000};

    nanosleep(&waitTime, NULL);
}

void runnable()
{
    for (int i = 0; i < 20; i++) {
        if (set_irql_2() != 0) {
            perror("runnable: interrupts_level_2");
            return;
        }
        printf("runnable: Everything locked ");
        usleep(500000);
        printf("and unlocked\n");
        if (set_irql_0() != 0) {
            perror("runnable: interrupts_level_0");
            return;
        }


        if (set_irql_1() != 0) {
            perror("runnable: interrupts_level_1");
            return;
        }

        printf("runnable: Only low priority locked ");
        usleep(500000);
        printf("and unlocked\n");
        if (set_irql_0() != 0) {
            perror("runnable: interrupts_level_0");
            return;
        }
    }
}

void usr1()
{
    printf("usr1: SIGUSR1 handler running.\n");

    for (int i = 0; i < 3; i++) {
        if (set_irql_2() != 0) {
            perror("runnable: set_irql_2");
            return;
        }

        printf("usr1: Everything locked for SIGUSR1 ");
        usleep(500000);
        printf("and unlocked\n");
        if (set_irql_1() != 0) {
            perror("runnable: set_irql_1");
            return;
        }
        usleep(100000);
    }
    printf("usr1: SIGUSR1 handler done.\n");
}

void usr2()
{
    printf("usr2: SIGUSR2 handler running.\n");
    usleep(500000);
    printf("usr2: SIGUSR2 handler done.\n");
}


int main()
{
    test_start("priority_interrupts.c");

    test_equals_int(run_with_signals(runnable, usr1, usr2), 0, "run_with_signals succeeds");

    return test_end();
}
