#include "dispatcher.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
 * We currently support 10 threads at maximum
 */
#define MAX_THREADS 10

/*
 * The stack size of a single user-level thread (4 KiB = 1 page)
 */
#define STACK_SIZE 4096

typedef enum _ThreadState {
    STATE_UNUSED = 0, // This entry in the _threads array is unused.
    STATE_READY,      // The thread is ready to run
    STATE_RUNNING     // The thread is currently running. _currentThread will
                      // hold the thread's id.

    // No waiting state so far.
} ThreadState;

typedef struct _Thread {
    /*
     * Current state of the thread. It can be:
     */
    ThreadState state;

    /*
     * The same as the index in the threads array.
     */
    int threadId;

    /*
     * The stack pointer of this thread while it is not running.
     */
    void *currentSP;
} Thread;

/*
 * This holds the index/id of the currently running user-level thread.
 */
int _currentThread;

Thread _threads[MAX_THREADS];

/*
 * Called once from main() to initialize our user-level thread implementation
 */
void initThreads()
{
    memset(_threads, 0, sizeof(_threads));

    // We use the user-mode part of the current kernel-level thread as the
    // first user-level thread. We therefore, do not need to setup a stack,
    // because the thread already possesses a stack.
    _currentThread = 0;

    _threads[0].state    = STATE_RUNNING;
    _threads[0].threadId = 0;
    // Cannot set stack pointer, because thread 0 is still running (and the
    // stack pointer is thus changing constantly). The stack pointer can only
    // be set in yield().
}

/*
 * When a thread exits its main function, it usually has to be terminated.
 * That is, its data structures including its stack have to be freed and the
 * thread needs to be removed from the scheduling. To ease our world, we let
 * the thread fall into parkThread() when it exits its main function.
 * parkThread() then lets the thread indefinitely yield() so we do not need to
 * perform any clean up. We thus do not really terminate the thread.
 */
void _parkThread()
{
    while (1) yield();
}

/*
 * Update the _currentThread variable to whatever thread should be next. This
 * function implements the scheduling policy for the user-level threads.
 */
static void _scheduleNextThread()
{
    assert(_threads[_currentThread].state == STATE_RUNNING);

    _threads[_currentThread].state = STATE_READY;

    // Simple O(n) round-robin scheduling policy. Find the next ready thread
    // and set it to running.
    do {
        _currentThread++;
        _currentThread %= MAX_THREADS;
    } while (_threads[_currentThread].state != STATE_READY);
    _threads[_currentThread].state = STATE_RUNNING;
}

/*
 * Switches to the next thread.
 */
void yield()
{
    // Apply a scheduling policy to decide which user-level thread to run next.
    // This will update _currentThread to the next thread!
    int prevThread = _currentThread;
    _scheduleNextThread();

    printf("Switch from thread %d with sp near %p\n",
        prevThread, &prevThread);
    printf("Switch to thread %d with sp=%p\n",
        _currentThread, _threads[_currentThread].currentSP);

    assert(_threads[prevThread].state == STATE_READY);
    assert(_threads[_currentThread].state == STATE_RUNNING);
    assert(_threads[_currentThread].currentSP != NULL);

    // Do the context switch
    __asm__ __volatile__ (
        // -------------- Implement context switch here ---------------

        // TODO: Implement the context switch using inline assembly.
        // You can use %%rsp, %%rax, %%r8, etc. to access a register
        // %[prevSp], %[newSp] get replaced by the previous and new thread
        // stack pointers

        "nop" // No-operation

        // ------------------------------------------------------------

        : [prevSp]"=m"(_threads[prevThread].currentSP)
        : [newSp]"m"(_threads[_currentThread].currentSP)
        : "cc", "memory");
}

/*
 * Prepares a new thread.
 * Returns the thread id or -1 on error.
 * This does not switch to the new thread, yet.
 */
int startThread(void (*func)(void))
{
    if (func == NULL) {
        return -1;
    }

    // Find a thread structure in our pre-allocated thread array, that we did
    // not use, yet.
    for (int i = 0; i < MAX_THREADS; ++i) {
        if (_threads[i].state == STATE_UNUSED) {

            // Allocate a stack for the user-level thread. For our purposes,
            // a simple, fixed-size memory region as stack is sufficient. We
            // can thus allocate stack space (i.e., memory space) from the
            // heap.
            void *stackBase = malloc(STACK_SIZE);
            if (stackBase == NULL) {
                return -1;
            }

            // Stacks grow from high addresses to low addresses. The stack thus
            // effectively starts at the end of the allocated memory area and
            // grows to the area's beginning.
            // We here assume the stack to be an array of void*'s, that is
            // pointers. A pointer has the right size (e.g., 32 or 64 bits) to
            // hold a full CPU register and can receive function pointers
            // without casting. stackTop is thus a pointer to pointers.
            void **stackTop = (void**)(stackBase + STACK_SIZE);

            // ------- Implement thread stack initialization here ---------

            // TODO: prepare the new stack, adjust stackTop.
            // Remember that stackTop-- decreases the stack pointer by 8 bytes.

            // ------------------------------------------------------------

            // We initialized the stack and the thread is ready to run.
            // After setting STATE_READY, the thread is eligible to dispatching
            _threads[i].currentSP = stackTop;
            _threads[i].state     = STATE_READY;

            return i;
        }
    }

    return -1;
}