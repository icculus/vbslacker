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

void __initMemoryManager(void *programFrame, void *globalData);
void __deinitMemoryManager(void);
void __initThreadMemoryManager(int tidx);
void __deinitThreadMemoryManager(int tidx);
void *__memAlloc(size_t amount);
void *__memRealloc(void *oldPtr, size_t amount);
void *__memAllocNoPtrs(size_t amount);
void *__memReallocNoPtrs(void *oldPtr, size_t amount);
void __memFree(void *ptr);
void __memDoPartialCollect(void);
void __memDoFullCollect(void);

#endif /* _INCLUDE_MEMORYMANAGER_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of MemoryManager.h ... */

