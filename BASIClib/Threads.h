/*
 * Declarations for abstract thread support in BASIClib.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_THREADS_H_
#define _INCLUDE_THREADS_H_

void __initThreads(void);
int __getThreadCount(void);
int __getCurrentThreadIndex(void);
int __getHighestThreadIndex(void);
void __threadTimeslice(void);
void __enterCriticalThreadSection(void);
void __exitCriticalThreadSection(void);

#endif

/* end of Threads.h ... */
