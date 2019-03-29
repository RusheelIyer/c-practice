#ifndef PRIORITY_INTERRUPTS_H
#define PRIORITY_INTERRUPTS_H

int run_with_signals(void (*runnable)(void), void (*usr1)(void),
    void (*usr2)(void));

int set_irql_0(void);
int set_irql_1(void);
int set_irql_2(void);

#endif

