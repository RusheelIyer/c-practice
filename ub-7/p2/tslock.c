#include "tslock.h"
#include <sched.h>

void tslock_init(TicketSpinlock *tslock)
{
    (void) tslock;
}

void tslock_lock(TicketSpinlock *tslock)
{
    (void) tslock;
}

void tslock_unlock(TicketSpinlock *tslock)
{
    (void) tslock;
}