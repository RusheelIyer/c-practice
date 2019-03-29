#include "tslock.h"
#include <sched.h>
#include <stdio.h>

void tslock_init(TicketSpinlock *tslock)
{
    if (tslock == NULL) {
        return;
    }

    tslock->currentTicket = 0;
    tslock->ticketCounter = 0;
}

void tslock_lock(TicketSpinlock *tslock)
{
    if (tslock == NULL) {
        return;
    }

    // atomic version of: int ticket = ticketCounter++;
    // We could use a gcc built-in for this:
    // ticket = __sync_fetch_and_add(&tslock->ticketCounter, 1);

    // However, in this assignment we have to use xadd together with lock
    // prefix to perform this task. Add ticket (register) and ticketCounter
    // (memory), write the result to ticketCounter and the old value of
    // ticketCounter to ticket. Therefore, we initialize ticket with 1.
    uint32_t ticket = 1;
    __asm__ volatile("lock xaddl %0, %1;"
        // We use 'ticket', which needs to be a register.
        : "=r" (ticket)
        // We also need the memory location of the ticket counter.
        // The memory location does not get changed, so this is just
        // an input parameter.
        : "m" (tslock->ticketCounter),
        // The ticket variable is both an input and output parameter.
        // In- and output are to the same register here, so we need to
        // tell the compiler.
        "0" (ticket)
        // This works as a memory barrier. No commands affecting memory
        // may be reordered around this assembly block.
        :"memory");

    // We wait until our thread gets the lock.
    while (tslock->currentTicket != ticket) {
        // Call yield to release the processor.
        sched_yield();
    }
}

void tslock_unlock(TicketSpinlock *tslock)
{
    if (tslock == NULL) {
        return;
    }

    // Increment the ticket variable. That way, the next thread can lock.
    tslock->currentTicket++;
}