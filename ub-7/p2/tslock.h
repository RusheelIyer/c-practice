#ifndef TSLOCK_H
#define TSLOCK_H

#include <inttypes.h>

typedef struct _TicketSpinlock {
    volatile uint32_t currentTicket; // Contains the current ticket value,
                                     // specifying which thread is currently
                                     // holding the spinlock

    volatile uint32_t ticketCounter; // Contains the next ticket value to
                                     // issue to a thread trying to acquire the
                                     // spin lock
} TicketSpinlock;

void tslock_init(TicketSpinlock *tslock);

void tslock_lock(TicketSpinlock *tslock);

void tslock_unlock(TicketSpinlock *tslock);

#endif