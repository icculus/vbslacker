/*
 * Header for internal memory management routines.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_INTERNALMEMMANAGER_H_
#define _INCLUDE_INTERNALMEMMANAGER_H_

#include <stdlib.h>
#include "RegState.h"

void __initInternalMemManager(STATEPARAMS);
void __deinitInternalMemManager(STATEPARAMS);

    /* Runtime-checked Memory allocation... */
void *__memAlloc(STATEPARAMS, size_t byteCount);
void *__memRealloc(STATEPARAMS, void *oldPtr, size_t byteCount);
void __memFree(STATEPARAMS, void *ptr);

    /* The "boxcar" system. */
void __memAllocBoxcar(STATEPARAMS, unsigned long boxcarId);
void *__memAllocInBoxcar(STATEPARAMS, unsigned long boxcarID, size_t byteCount);
void *__memReallocInBoxcar(STATEPARAMS, unsigned long boxcarId, void *ptr,
                           size_t byteCount);
void __memFreeInBoxcar(STATEPARAMS, unsigned long boxcarId, void *ptr);
void __memReleaseBoxcar(STATEPARAMS, unsigned long boxcarId);
void __memReleaseAllBoxcars(STATEPARAMS);
void __memReleaseBoxcarsBelow(STATEPARAMS, unsigned long boxcarId);
void __memReleaseBoxcarsAbove(STATEPARAMS, unsigned long boxcarId);

#endif

/* end of InternalMemManager.h ... */


