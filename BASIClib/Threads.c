/*
 * Abstract thread support for BASIClib, based on POSIX threads.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

/* !!! comment this thing. */

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




void __initThreads(void)
{
    __createThreadLock(&lock);
    threadCount++;
    indexes = __memAlloc(sizeof (pthread_t));

#ifndef WIN32
    indexes[0] = pthread_self();
#else
    indexes[0] = 15;
#endif

    __initThread(0);
} /* __initThreads */


void __deinitThreads(void)
{
    __destroyThreadLock(&lock);
} /* __deinitThreads */


void __prepareThreadToTerminate(int tidx)
{
    int i;
    int firstNULL = -1;

    __deinitThread(tidx);   /* notify other modules that thread is dying. */

    __obtainThreadLock(&lock);

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
        __memRealloc(indexes, firstNULL * sizeof (pthread_t));
    } /* if */

    __releaseThreadLock(&lock);
} /* __prepareThreadToTerminate */


void __terminateCurrentThread(void)
{
#ifndef WIN32
    __prepareThreadToTerminate(__getCurrentThreadIndex());
    pthread_exit(0);
#endif
} /* __terminateCurrentThread */


void __terminateThread(int tidx)
{
#ifndef WIN32
    pthread_t thread;

    __obtainThreadLock(&lock);
    thread = indexes[tidx];
    __releaseThreadLock(&lock);

    __prepareThreadToTerminate(tidx);
    pthread_kill(thread, SIGKILL);
#endif
} /* __terminateThread */


void __waitForThreadToDie(int tidx)
{
#ifndef WIN32
    pthread_t thread;

    __obtainThreadLock(&lock);
    thread = indexes[tidx];
    __releaseThreadLock(&lock);

    pthread_join(thread, NULL);
#endif
} /* __waitForThreadToDie */


void __threadEntry(void *_args)
{
    PThreadEntryArgs args = (PThreadEntryArgs) _args;

    __initThread(args->tidx);
    args->fn(args->args);
    __memFree(args);
    __terminateCurrentThread();
} /* __threadStart */


int __spinThread(void *(*_fn)(void *), void *_args)
{
#ifndef WIN32
    PThreadEntryArgs args = __memAlloc(sizeof (ThreadEntryArgs));
    pthread_t *saveLoc = NULL;
    int retVal;
    int rc;

    args->fn = _fn;
    args->args = _args;

    __obtainThreadLock(&lock);

    for (args->tidx = 0; args->tidx <= maxIndex; args->tidx++)
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

    retVal = args->tidx;
    rc = pthread_create(saveLoc, NULL, (void *) __threadEntry, args);

    __releaseThreadLock(&lock);

    if (rc != 0)
    {
        __memFree(args);
        retVal = -1;
        __runtimeError(ERR_INTERNAL_ERROR);
    } /* if */

    return(retVal);
#else
    return(-1);
#endif
} /* __spinThread */


int __getThreadCount(void)
{
    return(threadCount);
} /* __getThreadCount */


int __getHighestThreadIndex(void)
{
    return(maxIndex);
} /* __getHighestThreadIndex */


int __getCurrentThreadIndex(void)
{
#ifndef WIN32
    pthread_t tid = pthread_self();
    int i;
    int retVal = -1;

    __obtainThreadLock(&lock);

    for (i = 0; retVal == -1; i++)
    {
        if (indexes[i] == tid)
            retVal = i;
    } /* for */

    __releaseThreadLock(&lock);

    return(retVal);
#else
    return(0);
#endif
} /* __getCurrentThreadIndex */


void __threadTimeslice(void)
{
#ifndef WIN32
    (void) sched_yield();
#endif
} /* __threadTimeslice */


void __createThreadLock(PThreadLock pThreadLock)
{
#ifndef WIN32
    int errType;

    if (pthread_mutex_init(pThreadLock, NULL) != 0)
    { 
        errType = ((errno == ENOMEM) ? ERR_OUT_OF_MEMORY : ERR_INTERNAL_ERROR);
        __runtimeError(errType);
    } /* if */
#endif
} /* __createThreadLock */


void __destroyThreadLock(PThreadLock pThreadLock)
{
#ifndef WIN32
    if (pthread_mutex_destroy(pThreadLock) != 0)
        __runtimeError(ERR_INTERNAL_ERROR);
#endif
} /* __destroyThreadLock */


void __obtainThreadLock(PThreadLock pThreadLock)
{
#ifndef WIN32
    if (pthread_mutex_lock(pThreadLock) != 0)
        __runtimeError(ERR_INTERNAL_ERROR);
#endif
} /* __obtainThreadLock */


void __releaseThreadLock(PThreadLock pThreadLock)
{
#ifndef WIN32
    if (pthread_mutex_unlock(pThreadLock) != 0)
        __runtimeError(ERR_INTERNAL_ERROR);
#endif
} /* __releaseThreadLock */


/* end of Threads.c ... */

