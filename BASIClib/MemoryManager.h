/*
 * Header for internal memory management routines.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_MEMORYMANAGER_H_
#define _INCLUDE_MEMORYMANAGER_H_

#include <stdlib.h>  /* need size_t definition... */

void *__memAlloc(STATEPARAMS, size_t byteCount);
void *__memRealloc(STATEPARAMS, void *oldPtr, size_t byteCount);
void __memFree(STATEPARAMS, void *ptr);
void __forceFullBoxcarRelease(STATEPARAMS);
boolean __forcePartialBoxcarRelease(STATEPARAMS);

#endif
#endif

/* end of MemoryManager.h ... */

