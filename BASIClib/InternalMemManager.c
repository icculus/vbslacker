/*
 * Internal memory management routines. These aren't "BASIC" function
 *  calls, but BASIClib makes calls to these instead of the malloc()
 *  functions, and the parser/compiler might call them, too.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdlib.h>
#include "InternalMemManager.h"
#include "ErrorFunctions.h"

void *__memAlloc(size_t byteCount)
/*
 * Allocate a block of memory. Works like malloc(), except it throws
 *  a runtime error if there isn't enough memory for the allocation.
 *  Therefore this function never returns a NULL pointer.
 *
 *     params : byteCount == number of bytes to allocate.
 *    returns : pointer to newly allocated memory.
 */
{
    void *retVal;

    retVal = malloc(byteCount);
    if (retVal == NULL)
        __runtimeError(OUT_OF_MEMORY);
    
    return(retVal);
} /* __memAlloc */


void *__memRealloc(void *oldPtr, size_t byteCount)
/*
 * Allocate or resize a block of memory. Works like realloc(), except
 *  it throws a runtime error if there isn't enough memory for the
 *  (re)allocation. Therefore this function never returns a NULL pointer.
 *
 *     params : oldPtr == pointer to reallocate. NULL allocates a new block.
 *              byteCount == number of bytes to allocate.
 *    returns : pointer to newly allocated memory.
 */
{
    void *retVal;

    retVal = realloc(oldPtr, byteCount);
    if (retVal == NULL)
        __runtimeError(OUT_OF_MEMORY);

    return(retVal);
} /* __memRealloc */


void __memFree(void *ptr)
/*
 * Like free(), but only use this with pointers returned from
 *  __memAlloc() or __memRealloc()...we'll need this
 *  abstraction if we ever implement some sort of garbage collector...
 *
 *    params : ptr == pointer to block of memory to free.
 *   returns : void.
 */
{
    free(ptr);
} /* __memFree */


/* end of InternalMemManager.c ... */


