/*
 * Memory management routines.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdlib.h>
#include <malloc.h>
#include "MemoryManager.h"

#warning COMMENT THE MEMORYMANAGER!!!


    /*
     * ABOVESTACK is a comparison operator based on which
     *  direction the stack grows on a given platform...
     * STACKDECREMENT and STACKINCREMENT are macros that
     *  add (or subtract) in the way a pointer must move
     *  to go toward the the top of the stack.
     */
#if (STACK_DIRECTION == -1)
#    define ABOVESTACK <=
#    define STACKINCREMENT(x) ((x)--)
#    define STACKDECREMENT(x) ((x)++)
#else
#    define ABOVESTACK >=
#    define STACKINCREMENT(x) ((x)++)
#    define STACKDECREMENT(x) ((x)--)
#endif


void __initMemoryManager(void *programFrame, void *globalData)
/*
 * Initialize the memory manager.
 *
 *    params : programFrame == the lowest point on the stack; the base
 *                              pointer in main()...
 *   returns : void.
 */
{
} /* __initMemoryManager */


void __deinitMemoryManager(void)
/*
 * Initialize the memory manager.
 *
 *    params : void.
 *   returns : void.
 */
{
} /* __deinitMemoryManager */


void __initThreadMemoryManager(__integer tidx)
{
} /* __initThreadMemoryManager */


void __deinitThreadMemoryManager(__integer tidx)
{
} /* __initThreadMemoryManager */



void *__memAlloc(__long amount)
{
    void *retVal = malloc(amount);

    if (retVal == NULL)
        __runtimeError(ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memAlloc */


void *__memRealloc(void *oldPtr, __long amount)
{
    void *retVal = realloc(oldPtr, amount);

    if (retVal == NULL)
        __runtimeError(ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memRealloc */


void *__memAllocNoPtrs(__long amount)
{
    void *retVal = malloc(amount);

    if (retVal == NULL)
        __runtimeError(ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memAllocNoPtrs */


void *__memReallocNoPtrs(void *oldPtr, __long amount)
{
    void *retVal = realloc(oldPtr, amount);

    if (retVal == NULL)
        __runtimeError(ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memReallocNoPtrs */


void __memFree(void *ptr)
{
    free(ptr);
} /* __memFree */


void __memDoPartialCollect(void)
{
} /* __memDoPartialCollect */


void __memDoFullCollect(void)
{
} /* __memDoFullCollect */


/* end of MemoryManager.c ... */

