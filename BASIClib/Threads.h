/*
 * Declarations for abstract thread support in BASIClib.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_THREADS_H_
#define _INCLUDE_THREADS_H_

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

void __initThreads(void);
void __deinitThreads(void);
void __terminateCurrentThread(void);
void __terminateThread(int tidx);
void __waitForThreadToDie(int tidx);
int  __spinThread(void *(*_fn)(void *), void *_args);
int  __getThreadCount(void);
int  __getHighestThreadIndex(void);
int  __getCurrentThreadIndex(void);
void __threadTimeslice(void);
void __createThreadLock(PThreadLock pThreadLock);
void __destroyThreadLock(PThreadLock pThreadLock);
void __obtainThreadLock(PThreadLock pThreadLock);
void __releaseThreadLock(PThreadLock pThreadLock);

#endif

/* end of Threads.h ... */
