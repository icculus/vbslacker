/*
 * Header for internal memory management routines.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_MEMORYMANAGER_H_
#define _INCLUDE_MEMORYMANAGER_H_

#include <stdlib.h>  /* need size_t definition... */

void __initMemoryManager(void);
void __deinitMemoryManager(void);
void __initThreadMemoryManager(int tidx);
void __deinitThreadMemoryManager(int tidx);
void *__memAlloc(size_t byteCount);
void *__memRealloc(void *oldPtr, size_t byteCount);
void __memFree(void *ptr);
void *__memAllocInBoxcar(size_t byteCount);
void *__memReallocInBoxcar(void *ptr, int byteCount);
void __memFreeInBoxcar(void *ptr);
void __memForceFullBoxcarRelease(void);
__boolean __memForcePartialBoxcarRelease(void);

#endif /* _INCLUDE_MEMORYMANAGER_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of MemoryManager.h ... */

