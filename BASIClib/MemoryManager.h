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

#ifdef __cplusplus
extern "C" {
#endif

void __initMemoryManager(void *programFrame, void *globalData);
void __deinitMemoryManager(void);
void __initThreadMemoryManager(__integer tidx);
void __deinitThreadMemoryManager(__integer tidx);
void *__memAlloc(__long amount);
void *__memRealloc(void *oldPtr, __long amount);
void *__memAllocNoPtrs(__long amount);
void *__memReallocNoPtrs(void *oldPtr, __long amount);
void __memFree(void *ptr);
void __memDoPartialCollect(void);
void __memDoFullCollect(void);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_MEMORYMANAGER_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of MemoryManager.h ... */

