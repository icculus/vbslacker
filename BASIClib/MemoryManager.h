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

void __initMemoryManager(STATEPARAMS);
void __deinitMemoryManager(STATEPARAMS);
void __initThreadMemoryManager(STATEPARAMS, int tidx);
void __deinitThreadMemoryManager(STATEPARAMS, int tidx);
void *__memAlloc(STATEPARAMS, size_t byteCount);
void *__memRealloc(STATEPARAMS, void *oldPtr, size_t byteCount);
void __memFree(STATEPARAMS, void *ptr);
void *__memAllocInBoxcar(STATEPARAMS, size_t byteCount);
void *__memReallocInBoxcar(STATEPARAMS, void *ptr, int byteCount);
void __memFreeInBoxcar(STATEPARAMS, void *ptr);
void __memForceFullBoxcarRelease(STATEPARAMS);
__boolean __memForcePartialBoxcarRelease(STATEPARAMS);

#endif /* _INCLUDE_MEMORYMANAGER_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of MemoryManager.h ... */

