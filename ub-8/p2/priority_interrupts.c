#define _POSIX_SOURCE
#include "priority_interrupts.h"
#include <signal.h>
#include <unistd.h>
#include <assert.h>

int run_with_signals(void (*runnable)(void), void (*usr1)(void),
    void (*usr2)(void))
{
    (void) runnable;
    (void) usr1; // You may cast usr1 and usr2 to (void(*)(int)) if necessary.
    (void) usr2;
    return -1;
}

int set_irql_0(void)
{
    return -1;
}

int set_irql_1(void)
{
    return -1;
}

int set_irql_2(void)
{
    return -1;
}

