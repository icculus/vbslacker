/*
 * Header for internal memory management routines.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_INTERNALMEMMANAGER_H_
#define _INCLUDE_INTERNALMEMMANAGER_H_

#include <stdlib.h>

void __initInternalMemManager(void);
void __deinitInternalMemManager(void);

    /* Runtime-checked Memory allocation... */
void *__memAlloc(size_t byteCount);
void *__memRealloc(void *oldPtr, size_t byteCount);
void __memFree(void *ptr);

    /* The "boxcar" system. */
void __memAllocBoxcar(unsigned long boxcarId);
void *__memAllocInBoxcar(unsigned long boxcarID, size_t byteCount);
void *__memReallocInBoxcar(unsigned long boxcarId, void *ptr, size_t byteCount);
void __memFreeInBoxcar(unsigned long boxcarId, void *ptr);
void __memReleaseBoxcar(unsigned long boxcarId);
void __memReleaseAllBoxcars(void);
void __memReleaseBoxcarsBelow(unsigned long boxcarId);
void __memReleaseBoxcarsAbove(unsigned long boxcarId);

#endif

/* end of InternalMemManager.h ... */


