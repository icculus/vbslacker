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

#ifdef __cplusplus
extern "C" {
#endif

void __initMemoryManager(void);
void *__memAlloc(__long amount);
void *__memRealloc(void *oldPtr, __long amount);
void *__memAllocNoPtrs(__long amount);
void __memFree(void *ptr);
void __memDoFullCollect(void);
void __memDoPartialCollect(void);
void __memEnableGarbageCollector(__boolean shouldEnable);
__boolean __memIsGarbageCollectorEnabled(void);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_MEMORYMANAGER_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of MemoryManager.h ... */

