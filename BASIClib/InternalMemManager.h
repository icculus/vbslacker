/*
 * Header for internal memory management routines.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_INTERNALMEMMANAGER_H_
#define _INCLUDE_INTERNALMEMMANAGER_H_

#include <stdlib.h>

void *__memAlloc(size_t byteCount);
void *__memRealloc(void *oldPtr, size_t byteCount);
void __memFree(void *ptr);

#endif

/* end of InternalMemManager.h ... */


