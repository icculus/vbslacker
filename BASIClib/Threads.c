/*
 * Abstract thread support for BASIClib, based on POSIX threads.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#warning Comment Threads.c, please.

#include <stdlib.h>

#ifndef WIN32
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#endif

#include "InternalMemManager.h"
#include "ErrorFunctions.h"
#include "Threads.h"


typedef struct
{
    int tidx;
    void *(*fn)(void*);
    void *args;
} ThreadEntryArgs;

typedef ThreadEntryArgs *PThreadEntryArgs;



static ThreadLock lock;        /* module-scope thread lock. */
static pthread_t *indexes;
static volatile int threadCount = 0;
static volatile int maxIndex = 0;



    /* defined in Initialize.c, but only declared here for abstractness. */
void __initThread(int tidx); 
void __deinitThread(int tidx); 




void __initThreads(STATEPARAMS)
{
    __createThreadLock(STATEARGS, &lock);
    threadCount++;
    indexes = __memAlloc(STATEARGS, sizeof (pthread_t));

#ifndef WIN32
    indexes[0] = pthread_self();
#else
    indexes[0] = 15;
#endif

    __initThread(STATEARGS, 0);
} /* __initThreads */


void __deinitThreads(STATEPARAMS)
{
    __destroyThreadLock(STATEARGS, &lock);
} /* __deinitThreads */


void __prepareThreadToTerminate(STATEPARAMS, int tidx)
{
    int i;
    int firstNULL = -1;

    __deinitThread(STATEARGS, tidx);   /* broadcast that thread is dying. */

    __obtainThreadLock(STATEARGS, &lock);

    threadCount--;

    indexes[tidx] = (pthread_t) NULL;
    for (i = 0; i <= maxIndex; i++)
    {
        if (indexes[i] != (pthread_t) NULL)
            firstNULL = -1;
        else if (firstNULL == -1)
            firstNULL = i;
    } /* for */

    if (firstNULL != -1)
    {
        maxIndex = firstNULL - 1;
        __memRealloc(STATEARGS, indexes, firstNULL * sizeof (pthread_t));
    } /* if */

    __releaseThreadLock(STATEARGS, &lock);
} /* __prepareThreadToTerminate */


void __terminateCurrentThread(STATEPARAMS)
{
#ifndef WIN32
    __prepareThreadToTerminate(STATEARGS, __getCurrentThreadIndex(STATEARGS));
    pthread_exit(0);
#endif
} /* __terminateCurrentThread */


void __terminateThread(STATEPARAMS, int tidx)
{
#ifndef WIN32
    pthread_t thread;

    __obtainThreadLock(STATEARGS, &lock);
    thread = indexes[tidx];
    __releaseThreadLock(STATEARGS, &lock);

    __prepareThreadToTerminate(STATEARGS, tidx);
    pthread_kill(thread, SIGKILL);
#endif
} /* __terminateThread */


void __waitForThreadToDie(STATEPARAMS, int tidx)
{
#ifndef WIN32
    pthread_t thread;

    __obtainThreadLock(STATEARGS, &lock);
    thread = indexes[tidx];
    __releaseThreadLock(STATEARGS, &lock);

    pthread_join(thread, NULL);
#endif
} /* __waitForThreadToDie */


void __kickOffThread(STATEPARAMS, PThreadEntryArgs args)
{
    __initThread(STATEARGS, args->tidx);
    args->fn(STATEARGS, args->args);
    __memFree(STATEARGS, args);
    __terminateCurrentThread(STATEARGS);
} /* __kickOffThread */


void __threadEntry(void *_args)
{
    __kickOffThread(NULLSTATEARGS, (PThreadEntryArgs) _args);
} /* __threadStart */


int __spinThread(STATEPARAMS, void *(*_fn)(void *), void *_args)
{
#ifndef WIN32
    PThreadEntryArgs args = __memAlloc(STATEARGS, sizeof (ThreadEntryArgs));
    pthread_t *saveLoc = NULL;
    int retVal;
    int rc;

    args->fn = _fn;
    args->args = _args;

    __obtainThreadLock(STATEARGS, &lock);

    for (args->tidx = 0; args->tidx <= maxIndex; args->tidx++)
    {
        if (indexes[args->tidx] == (pthread_t) NULL)
            saveLoc = &indexes[args->tidx];
    } /* for */

    if (saveLoc == NULL)    /* no space in table? */
    {
        maxIndex++;
        indexes = __memRealloc(STATEARGS, indexes,
                                (maxIndex + 1) * sizeof (pthread_t));
        saveLoc = &indexes[maxIndex];
        args->tidx = maxIndex;
    } /* if */

    retVal = args->tidx;
    rc = pthread_create(saveLoc, NULL, (void *) __threadEntry, args);

    __releaseThreadLock(STATEARGS, &lock);

    if (rc != 0)
    {
        __memFree(STATEARGS, args);
        retVal = -1;
        __runtimeError(STATEARGS, ERR_INTERNAL_ERROR);
    } /* if */

    return(retVal);
#else
    return(-1);
#endif
} /* __spinThread */


int __getThreadCount(STATEPARAMS)
{
    return(threadCount);
} /* __getThreadCount */


int __getHighestThreadIndex(STATEPARAMS)
{
    return(maxIndex);
} /* __getHighestThreadIndex */


int __getCurrentThreadIndex(STATEPARAMS)
{
#ifndef WIN32
    pthread_t tid = pthread_self();
    int i;
    int retVal = -1;

    __obtainThreadLock(STATEARGS, &lock);

    for (i = 0; retVal == -1; i++)
    {
        if (indexes[i] == tid)
            retVal = i;
    } /* for */

    __releaseThreadLock(STATEARGS, &lock);

    return(retVal);
#else
    return(0);
#endif
} /* __getCurrentThreadIndex */


void __threadTimeslice(STATEPARAMS)
{
#ifndef WIN32
    (void) sched_yield();
#endif
} /* __threadTimeslice */


void __createThreadLock(STATEPARAMS, PThreadLock pThreadLock)
{
#ifndef WIN32
    int errType;

    if (pthread_mutex_init(pThreadLock, NULL) != 0)
    { 
        errType = ((errno == ENOMEM) ? ERR_OUT_OF_MEMORY : ERR_INTERNAL_ERROR);
        __runtimeError(STATEARGS, errType);
    } /* if */
#endif
} /* __createThreadLock */


void __destroyThreadLock(STATEPARAMS, PThreadLock pThreadLock)
{
#ifndef WIN32
    if (pthread_mutex_destroy(pThreadLock) != 0)
        __runtimeError(STATEARGS, ERR_INTERNAL_ERROR);
#endif
} /* __destroyThreadLock */


void __obtainThreadLock(STATEPARAMS, PThreadLock pThreadLock)
{
#ifndef WIN32
    if (pthread_mutex_lock(pThreadLock) != 0)
        __runtimeError(STATEARGS, ERR_INTERNAL_ERROR);
#endif
} /* __obtainThreadLock */


void __releaseThreadLock(STATEPARAMS, PThreadLock pThreadLock)
{
#ifndef WIN32
    if (pthread_mutex_unlock(pThreadLock) != 0)
        __runtimeError(STATEARGS, ERR_INTERNAL_ERROR);
#endif
} /* __releaseThreadLock */


/* end of Threads.c ... */

