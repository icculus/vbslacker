/*
 * Abstract thread support for BASIClib.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

/* !!! comment this thing. */

#include <stdlib.h>
#include "Threads.h"

static volatile int criticalSectionCount = 0;
static volatile int *indexes;
static volatile int threadCount = 0;

    /* defined in Initialize.c, but only declared here for abstractness. */
void __initThread(int tidx); 
void __deinitThread(int tidx); 

void __initThreads(void)
{
    threadCount++;
    indexes = malloc(sizeof (int));
    /* !!! indexes[0] = __OS__getTID(); */

    __newThreadInit(0);
} /* __initThreads */

int __getThreadCount(void)
{
    return(threadCount);
} /* __getThreadCount */

int __getHighestThreadIndex(void)
{
    return(0);
} /* __getHighestThreadIndex */

int __getCurrentThreadIndex(void)
{
    return(0);
} /* __getCurrentThreadIndex */

void __threadTimeslice(void)
{
} /* __threadTimeslice */


void __enterCriticalThreadSection(void)
{
    criticalSectionCount++;
} /* __enterCriticalThreadSection */


void __exitCriticalThreadSection(void)
{
    criticalSectionCount--;
    if (criticalSectionCount == 0)
        ; /* do something. */
} /* __exitCriticalThreadSection */


/* end of Threads.c ... */

