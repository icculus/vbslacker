/*
 * Abstract thread support for BASIClib.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "Threads.h"

/* !!! comment this thing. */

int __getThreadCount(void)
{
    return(1);
} /* __getThreadCount */

int __getCurrentThreadIndex(void)
{
    return(0);
} /* __getCurrentThreadIndex */

void __threadTimeslice(void)
{
} /* __threadTimeslice */

void __enterCriticalThreadSection(void)
{
} /* __enterCriticalThreadSection */

void __exitCriticalThreadSection(void)
{
} /* __exitCriticalThreadSection */


/* end of Threads.c ... */

