/*
 * Abstract thread support for BASIClib, based on POSIX threads.
 *
 * A brief note: POSIX defines a "tid" (Thread ID...they call it a
 *  "pthread_t", which is basically a handle, which is basically an ID
 *  number) for every thread. BASIClib defines a "tidx" (Thread InDeX),
 *  which is always from 0 to threadCount-1...give or take. We keep all
 *  the tids in a vector. Their position in the vector is their tidx. This
 *  allows a little more abstraction (in case we drop POSIX for something
 *  else...god knows what...) and simplifies end-user and BASIClib code.
 *  A good example exists in the resizing of an array of thread-specific
 *  data in BasicError.c's __initThreadBasicError() function. Every function
 *  provided here expects a tidx, not a tid. Outside code doesn't even
 *  know we're using POSIX threads, or Win32 threads, or whatnot...
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "Threads.h"

#if (!defined SINGLE_THREADED)
#   include <stdlib.h>
#   include <string.h>
#   include <pthread.h>
#   include <sched.h>
#   include <signal.h>

    /*
     * This structure contains the arguments to be
     *  passed via a (void *) to the entry function of
     *  a newly-spun thread...
     */
    typedef struct
    {
        int tidx;                  /* New thread's BASIClib index. */
        void (*fn)(void *args);    /* Function to call.            */
        void *args;                /* Args to function to call.    */
    } ThreadEntryArgs;

    typedef ThreadEntryArgs *PThreadEntryArgs;

    static ThreadLock lock;              /* module-scope thread lock.        */
    static pthread_t *indexes;           /* Vector of actual TIDs...         */
    static int threadCount = 0; /* Count of total existing threads. */
    static int maxIndex = -1;   /* Size of (indexes) vector.        */
#endif /* !defined SINGLE_THREADED */


/* defined in Initialize.c, but only declared here for abstractness. */
void __initThread(int tidx);
void __deinitThread(int tidx);



void __initThreads(void)
/*
 * Basic initializations for the Threads module. Sets up internal
 *  data, and calls __initThread() on the current tidx (0), which
 *  notifies all the other modules that a thread now exists.
 *
 *     params : void.
 *    returns : void.
 */
{
#if (!defined SINGLE_THREADED)
    __createThreadLock(&lock);
    threadCount++;
    maxIndex++;
    indexes = __memAlloc(sizeof (pthread_t));

    indexes[0] = pthread_self();
#endif

    __initThread(0);
} /* __initThreads */


void __deinitThreads(void)
/*
 * Shut down the Threads module. Terminate all but the current thread,
 *  notifying all the other modules, and cleanup details such as our
 *  ThreadLock.
 *
 *     params : void.
 *    returns : void.
 */
{
#if (!defined SINGLE_THREADED)
    int i;
    pthread_t tid = pthread_self();

    __obtainThreadLock(&lock);

    for (i = 0; i < maxIndex; i++)
    {
        if ((indexes[i] != (pthread_t) NULL) && (indexes[i] != tid))
            __terminateThread(indexes[i]);
    } /* for */

    __releaseThreadLock(&lock);

    __destroyThreadLock(&lock);

#else
    /*
     * This is done in the __terminateThread() loop
     *  above for multithread mode...
     */
    __deinitThread(0);
#endif
} /* __deinitThreads */


    /*
     * The rest of these functions should only exist if
     *  we aren't compiled as a single-threaded library.
     */
#if (!defined SINGLE_THREADED)

static void __prepareThreadToTerminate(int tidx)
/*
 * Sets up a thread for termination. Notifies other modules that
 *  thread is dying, and possibly resizes the vector containing TIDs.
 *
 * This function is called whether a thread is terminating normally
 *  or not.
 *
 *     params : tidx == index of thread that is to terminate.
 *    returns : void.
 */
{
    int i;
    int firstNULL = -1;
    pthread_t tid;

    __deinitThread(tidx);   /* broadcast that thread is dying. */

    __obtainThreadLock(&lock);

    threadCount--;

    tid = indexes[tidx];
    indexes[tidx] = (pthread_t) NULL;
    for (i = maxIndex; (i >= 0) && (firstNULL == -1); i--)
    {
        if (indexes[i] == (pthread_t) NULL)
            firstNULL = i;
    } /* for */

    if (firstNULL != -1)
    {
        maxIndex = firstNULL - 1;
        __memRealloc(indexes, firstNULL * sizeof (pthread_t));
    } /* if */

    pthread_detach(tid);
    __releaseThreadLock(&lock);
} /* __prepareThreadToTerminate */


void __terminateCurrentThread_f(void)
/*
 * Make the thread that called this function commit suicide.
 *
 *    params : void.
 *   returns : theoretically, never.
 */
{
    __prepareThreadToTerminate(__getCurrentThreadIndex_f());
    pthread_exit(0);
} /* __terminateCurrentThread_f */


void __terminateThread_f(int tidx)
/*
 * Terminate a thread. If the calling thread is the one that
 *  is requested to terminate, the function __terminateCurrentThread()
 *  is called.
 *
 *      params : tidx == index of thread to terminate.
 *     returns : void. if (tidx == current thread), never returns.
 */
{
    pthread_t thread;

    if (tidx == __getCurrentThreadIndex_f())
        __terminateCurrentThread_f();
    else
    {
        __obtainThreadLock_f(&lock);
        if (tidx <= maxIndex)
            thread = indexes[tidx];
        __releaseThreadLock_f(&lock);

        /* !!! suspend thread? */
        __prepareThreadToTerminate(tidx);
        pthread_kill(thread, SIGKILL);
    } /* else */
} /* __terminateThread_f */


void __waitForThreadToDie_f(int tidx)
/*
 * This function blocks until thread with the index (tidx)
 *  terminates. Calling this when (tidx) is the current thread
 *  has undefined behavior.
 *
 *      params : tidx == index of thread we're waiting to die.
 *     returns : void.
 */
{
    pthread_t thread = 0;

    __obtainThreadLock_f(&lock);
    if (tidx <= maxIndex)
        thread = indexes[tidx];
    __releaseThreadLock_f(&lock);

    if (thread != 0)
        pthread_join(thread, NULL);
} /* __waitForThreadToDie_f */


static void __threadEntry(void *args)
/*
 * This is a simple entry point from __spinThread().
 *
 * This is where other modules are notified of the new thread's
 *  creation. Thus, all __initThreadMyModule() calls run from inside
 *  the newly created thread, before the thread starts its function.
 *
 *     params : args == points to a PThreadEntryArgs.
 *    returns : void.
 */
{
    __initThread(((PThreadEntryArgs) args)->tidx);
    ((PThreadEntryArgs) args)->fn(((PThreadEntryArgs) args)->args);
    __memFree(args);
    __terminateCurrentThread_f();
} /* __threadStart */


int __spinThread_f(void (*_fn)(void *x), void *_args)
/*
 * Create ("spin") a new thread, and start it.
 *
 *    params : _fn == ptr to function to execute in this thread.
 *             _args == arguments to (_fn).
 *   returns : index of new thread. -1 on error.
 */
{
    PThreadEntryArgs args = __memAlloc(sizeof (ThreadEntryArgs));
    pthread_t *saveLoc = NULL;
    int retVal;
    int rc;

    args->fn = _fn;
    args->args = _args;

    __obtainThreadLock_f(&lock);

    for (args->tidx = 0; (args->tidx <= maxIndex) && (saveLoc == NULL);
            args->tidx++)
    {
        if (indexes[args->tidx] == (pthread_t) NULL)
            saveLoc = &indexes[args->tidx];
    } /* for */

    if (saveLoc == NULL)    /* no space in table? */
    {
        maxIndex++;
        indexes = __memRealloc(indexes, (maxIndex + 1) * sizeof (pthread_t));
        saveLoc = &indexes[maxIndex];
        args->tidx = maxIndex;
    } /* if */
    else
        args->tidx--;

    threadCount++;

    retVal = args->tidx;
    rc = pthread_create(saveLoc, NULL, (void *) __threadEntry, (void *) args);

    __releaseThreadLock_f(&lock);

    if (rc != 0)
    {
        __memFree(args);
        retVal = -1;
        __runtimeError(ERR_INTERNAL_ERROR);
    } /* if */

    return(retVal);
} /* __spinThread_f */


int __getThreadCount_f(void)
/*
 * Return count of current amount of threads in system. More handy is
 *  probably __getHighestThreadIndex(), below.
 *
 *     params : void.
 *    returns : Current count of existing threads. Can be inaccurate
 *              before you get the return value.
 */
{
    return(threadCount);
} /* __getThreadCount_f */


int __getHighestThreadIndex_f(void)
/*
 * Get the index of the thread with the highest-numbered index. Good for
 *  determining how to size arrays that handle thread-specific data...
 *
 *    params : void.
 *   returns : highest thread index in use.
 */
{
    return(maxIndex);
} /* __getHighestThreadIndex_f */


int __getCurrentThreadIndex_f(void)
/*
 * Get the index of the current calling thread.
 *
 *     params : void.
 *    returns : calling thread's index.
 */
{
    pthread_t tid = pthread_self();
    int i;
    int retVal = -1;

    __obtainThreadLock_f(&lock);

    for (i = 0; retVal == -1; i++)
    {
        if (indexes[i] == tid)
            retVal = i;
    } /* for */

    __releaseThreadLock_f(&lock);

    return(retVal);
} /* __getCurrentThreadIndex_f */


void __threadTimeslice_f(void)
/*
 * Yield a timeslice to the system. Use this in idle-loops and other
 *  processor-intensive things. The amount of time yielded before
 *  control returns to the calling thread is undefined, but probably
 *  more than bareable.
 *
 *      params : void.
 *     returns : void.
 */
{
    (void) sched_yield();
} /* __threadTimeslice_f */


void __createThreadLock_f(PThreadLock pThreadLock)
/*
 * Initialize a ThreadLock variable for use. Call this on a given
 *  ThreadLock before using it, or it won't work, or maybe'll crash
 *  the application.
 *
 *     params : pThreadLock == ptr to ThreadLock to initialize.
 *    returns : void.
 */
{
    int errType;
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    attr.mutexkind = PTHREAD_MUTEX_RECURSIVE_NP;

    if (pthread_mutex_init(pThreadLock, &attr) != 0)
    { 
        errType = ((errno == ENOMEM) ? ERR_OUT_OF_MEMORY : ERR_INTERNAL_ERROR);
        __runtimeError(errType);
    } /* if */

    pthread_mutexattr_destroy(&attr);
} /* __createThreadLock_f */


void __destroyThreadLock_f(PThreadLock pThreadLock)
/*
 * Call this on a ThreadLock after you are done with it. This
 *  function will free the resources associated with it.
 *
 * A ThreadLock must be run through  __createThreadLock() again
 *  after this function before it can be used again.
 *
 *     params : pThreadLock == ptr to ThreadLock to deinitialize.
 *    returns : void.
 */
{
    if (pthread_mutex_destroy(pThreadLock) != 0)
        __runtimeError(ERR_INTERNAL_ERROR);
} /* __destroyThreadLock_f */


void __obtainThreadLock_f(PThreadLock pThreadLock)
/*
 * Request control of a ThreadLock. Any other threads that call
 *  this function before the calling thread calls __releaseThreadLock()
 *  will block until the ThreadLock is released. Make ample (but wise)
 *  use of this function to prevent race conditions in your code.
 *
 *     params : pThreadLock == ptr to ThreadLock to obtain.
 *    returns : void.
 */
{
    if (pthread_mutex_lock(pThreadLock) != 0)
        __runtimeError(ERR_INTERNAL_ERROR);
} /* __obtainThreadLock_f */


void __releaseThreadLock_f(PThreadLock pThreadLock)
/*
 * Release control of a ThreadLock you control. This will allow one
 *  of any blocked threads that are requesting control of this ThreadLock
 *  to gain control and unblock.
 *
 *      params : pThreadLock == ThreadLock to release control of.
 *     returns : void.
 */
{
    if (pthread_mutex_unlock(pThreadLock) != 0)
        __runtimeError(ERR_INTERNAL_ERROR);
} /* __releaseThreadLock_f */

#endif /* (!defined SINGLE_THREADED) */

/* end of Threads.c ... */

