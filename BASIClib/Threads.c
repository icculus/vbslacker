/*
 * Abstract thread support for BASIClib.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

/* !!! comment this thing. */

#include <stdlib.h>
#include <pthread.h>
#include "Threads.h"

typedef struct
{
    void *(*fn)(void*);
    void *args;
} ThreadEntryArgs;

typedef ThreadEntryArgs *PThreadEntryArgs;

static volatile ThreadLock lock;
static volatile pthread_t *indexes;
static volatile int threadCount = 0;
static volatile int maxIndex = 0;

    /* defined in Initialize.c, but only declared here for abstractness. */
void __initThread(int tidx); 
void __deinitThread(int tidx); 


void __initThreads(void)
{
    __createThreadLock(&lock);
    threadCount++;
    maxIndex++;
    indexes = malloc(sizeof (pthread_t));
    indexes[0] = pthread_self();

    __initThread(0);
} /* __initThreads */


void __deinitThreads(void)
{
    __destroyThreadLock(&lock);
} /* __deinitThreads */


void __threadEntry(void *_args)
{
    PThreadStartArgs args = (PThreadEntryArgs) _args;

    __initThread(args->tidx);
    args->fn(args->args);
    __memFree(args);
    __terminateCurrentThread();
} /* __threadStart */


int __spinThread(void *(*_fn)(void *), void *_args)
{
    int i;
    PThreadEntryArgs args = __memAlloc(sizeof (ThreadEntryArgs));
    args->fn = _fn;
    args->args = _args;

    __obtainThreadLock(&lock);
    for (i = 0; i < maxIndex; i++)
    {
        if (indexes[i] == NULL)
            !!!
    } /* for */

    __releaseThreadLock(&lock);
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
    pthread_t tid = pthread_self();
    int retVal = -1;

    __obtainThreadLock(&lock);

    for (i = 0; retVal == -1; i++)
    {
        if (indexes[i] == tid)
            retVal = i;
    } /* for */

    __releaseThreadLock(&lock);

    return(retVal);
} /* __getCurrentThreadIndex */


void __threadTimeslice(void)
{
    /* !!! ?! */
} /* __threadTimeslice */


void __createThreadLock(PThreadLock pThreadLock)
{
    int errType;

    if (pthread_mutex_init(pThreadLock, NULL) != 0)
    { 
        errType = ((errno == ENOMEM) ? ERR_OUT_OF_MEMORY : ERR_INTERNAL_ERROR);
        __runtimeError(errType);
    } /* if */
} /* __createThreadLock */


void __destroyThreadLock(PThreadLock pThreadLock)
{
    if (pthread_mutex_destroy(pThreadLock) != 0)
        __runtimeError(ERR_INTERNAL_ERROR);
} /* __destroyThreadLock */


void __obtainThreadLock(PThreadLock pThreadLock)
{
    if (pthread_mutex_lock(pThreadLock) != 0)
        __runtimeError(ERR_INTERNAL_ERROR);
} /* __obtainThreadLock */


void __releaseThreadLock(PThreadLock pThreadLock)
{
    if (pthread_mutex_unlock(pThreadLock) != 0)
        __runtimeError(ERR_INTERNAL_ERROR);
} /* __releaseThreadLock */


/* end of Threads.c ... */

