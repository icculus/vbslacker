/*
 * Memory management routines.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 *
 *  This module makes calls to the Boehm-Demers-Weiser garbage
 *  collection engine. Here is their disclaimer:
 *
 *    Copyright 1988, 1989 Hans-J. Boehm, Alan J. Demers
 *    Copyright (c) 1991-1996 by Xerox Corporation.  All rights reserved.
 *    Copyright (c) 1996 by Silicon Graphics.  All rights reserved.
 *
 *    THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
 *    OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
 */

#include <stdlib.h>
#include "gc.h"
#include "MemManager.h"


void *__memAlloc(STATEPARAMS, size_t byteCount)
/*
 * Allocate a block of memory. Works like malloc(), except it throws
 *  a runtime error if there isn't enough memory for the allocation.
 *  Therefore this function never returns a NULL pointer.
 *
 *  (Well, it's not ENTIRELY like malloc(). This actually uses the
 *   garbage collector's allocation routines...)
 *
 *     params : byteCount == number of bytes to allocate.
 *    returns : pointer to newly allocated memory.
 */
{
    void *retVal;

    retVal = GC_malloc(byteCount);
    if (retVal == NULL)
        __runtimeError(STATEARGS, ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memAlloc */


void *__memRealloc(STATEPARAMS, void *oldPtr, size_t byteCount)
/*
 * Allocate or resize a block of memory. Works like realloc(), except
 *  it throws a runtime error if there isn't enough memory for the
 *  (re)allocation. Therefore this function never returns a NULL pointer.
 *
 *  (Well, it's not ENTIRELY like malloc(). This actually uses the
 *   garbage collector's allocation routines...)
 *
 *     params : oldPtr == pointer to reallocate. NULL allocates a new block.
 *              byteCount == number of bytes to allocate.
 *    returns : pointer to newly allocated memory.
 */
{
    void *retVal;

    retVal = GC_realloc(oldPtr, byteCount);
    if (retVal == NULL)
        __runtimeError(STATEARGS, ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memRealloc */


#ifdef WANT_EXTRA_GC_API

void *__memAllocNoPointers(STATEPARAMS, size_t byteCount)
/*
 * Use this when allocating memory that you can guarantee will not be
 *  holding pointers to other allocated memory. The garbage collector
 *  will never evaluate the contents of this allocated block for
 *  references to objects, so objects may be incorrectly considered
 *  garbage if pointers to it reside in memory allocated by this
 *  function. However, used correctly, this speeds up collection,
 *  since the collector knows to never examine these blocks.
 *
 * With that distinction, this function works like __memAlloc()...
 *
 *     params : byteCount == count of bytes to allocate.
 *    returns : pointer to newly allocated memory.
 */
{
    void *retVal;

    retVal = GC_malloc_atomic(oldPtr, byteCount);
    if (retVal == NULL)
        __runtimeError(STATEARGS, ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memAllocNoPointers */


void __memFree(STATEPARAMS, void *ptr)
/*
 * Like free(), but only use this with pointers returned from
 *  __memAlloc() or __memRealloc()...this not only deallocates
 *  a memory object, but it notifies the garbage collector that
 *  the object has been removed potentially before it would have
 *  been marked as garbage.
 *
 *    params : ptr == pointer to block of memory to free.
 *   returns : void.
 */
{
#warning __memFree() is currently a no-op.
} /* __memFree */


void __forceFullMemCleanup(STATEPARAMS)
/*
 * Force the garbage collector to reclaim every possible bit of
 *  memory. Could potentially take awhile to run, and blocks until
 *  complete.
 *
 *     params : void.
 *    returns : void.
 */
{
#warning __forceFullGarbageCollection() is currently a no-op.
} /* __forceFullGarbageCollection */


boolean __forcePartialMemCleanup(STATEPARAMS)
/*
 * Force the garbage collector to reclaim whatever garbage can be
 *  cleaned up in a relatively timely manner. Good to call this in
 *  an idle loop, so collection is done a little at a time until
 *  either the loop exits, or all garbage has been collected.
 *
 *     params : void.
 *    returns : boolean (true) if garbage remains, boolean (false) otherwise.
 */
{
#warning __forcePartialGarbageCollection() is currently a no-op.
} /* __forcePartialGarbageCollection */

#endif

/* end of MemoryManager.c ... */


