/*
 * Declarations for abstract thread support in BASIClib.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_THREADS_H_
#define _INCLUDE_THREADS_H_

#include "RegState.h"

#ifndef _REENTRANT
#error _REENTRANT not defined!
#error Please put -D_REENTRANT on the command line.
#error  otherwise, the C library is NOT thread-safe!
#endif


#ifndef WIN32

#include <pthread.h>

#else
    typedef unsigned long pthread_t;
    typedef unsigned long pthread_mutex_t;
#endif


#define ThreadLock pthread_mutex_t
#define PThreadLock pthread_mutex_t *

void __initThreads(STATEPARAMS);
void __deinitThreads(STATEPARAMS);
void __terminateCurrentThread(STATEPARAMS);
void __terminateThread(STATEPARAMS, int tidx);
void __waitForThreadToDie(STATEPARAMS, int tidx);
int  __spinThread(STATEPARAMS, void *(*_fn)(STATEPARAMS, void *x), void *_args);
int  __getThreadCount(STATEPARAMS);
int  __getHighestThreadIndex(STATEPARAMS);
int  __getCurrentThreadIndex(STATEPARAMS);
void __threadTimeslice(STATEPARAMS);
void __createThreadLock(STATEPARAMS, PThreadLock pThreadLock);
void __destroyThreadLock(STATEPARAMS, PThreadLock pThreadLock);
void __obtainThreadLock(STATEPARAMS, PThreadLock pThreadLock);
void __releaseThreadLock(STATEPARAMS, PThreadLock pThreadLock);

#endif

/* end of Threads.h ... */

