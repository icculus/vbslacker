/*
 * Memory management routines.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#include "gc.h"
#include "MemoryManager.h"

#define VBSTARTHEAP  "VBSTARTHEAP"


static void increaseDefaultHeap(void)
{
    char *env = getenv(VBSTARTHEAP);
    char *increaseByChar = NULL;
    __long heapIncrease = 0;

    if (env != NULL)
    {
        heapIncrease = atoi(env);
        increaseByChar = env + strlen(env) - 1;

        switch (tolower(*increaseByChar))
        {
            case 'k':
                heapIncrease *= 1024;
                break;

            case 'm':
                heapIncrease *= 1024 * 1024;
                break;

            case 'g':
                heapIncrease *= 1024 * 1024 * 1024;
                break;
        } /* switch */

        if (heapIncrease > 0)
        {
                /* !!! should this be fatal? */
            if (GC_expand_hp(heapIncrease) == 0) /* 0 == failed */
                __runtimeError(ERR_OUT_OF_MEMORY);
        } /* if */
    } /* if */
} // increaseDefaultHeap


// jillian@giftedandtalented.org


void __initMemoryManager(void)
/*
 * Initialize the memory manager.
 *
 *    params : void.
 *   returns : void.
 */
{
    GC_INIT();
    GC_max_retries = 1;
    __memEnableGarbageCollector(true);
    increaseDefaultHeap();
} /* __initMemoryManager */


void *__memAlloc(__long amount)
/*
 * Allocate a block of memory (a "memory object"). This works just like
 *  malloc(), except you don't need to explicitly free() the memory when
 *  you are done with it (although you may use __memFree() on it.)
 *
 * Allocated blocks are NOT guaranteed to be blanked out with NULL chars.
 *
 * This function never returns NULL. If memory cannot be allocated, the
 *  garbage collector runs a world-stopping full collection once, and tries
 *  allocation again. If memory still cannot be allocated, the runtime error
 *  ERR_OUT_OF_MEMORY (#7) is thrown.
 *
 * Do not use free() or realloc() on pointers returned by this function.
 *  Use __memFree() and __memRealloc() if you must (neither are required
 *  in a garbage collecting paradigm.)
 *
 * All other rules of garbage collection apply to this. Don't store pointers
 *  to disk, make sure you NULL out the pointer when done with it, etc...
 *
 *      params : amount == amount of bytes to allocate.
 *     returns : pointer to newly allocated block. Never returns NULL.
 */
{
    void *retVal = GC_malloc(amount);

    if (retVal == NULL)
        __runtimeError(ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memAlloc */


void *__memRealloc(void *oldPtr, __long amount)
/*
 * Reallocate a block of memory (a "memory object"). This works just like
 *  realloc(), except you don't need to explicitly free() the memory when
 *  you are done with it (although you may use __memFree() on it.)
 *
 * Reallocated blocks contain the previous block's contents. The contents
 *  of the newly allocated portion of the object are undefined.
 *
 * You may specify (NULL) for (oldPtr), just like in realloc(). The entire
 *  object's contents are undefined if you do this.
 *
 * This function never returns NULL. If memory cannot be allocated, the
 *  garbage collector runs a world-stopping full collection once, and tries
 *  allocation again. If memory still cannot be allocated, the runtime error
 *  ERR_OUT_OF_MEMORY (#7) is thrown.
 *
 * Do not use free() or realloc() on pointers returned by this function.
 *  Use __memFree() and __memRealloc() if you must (neither are required
 *  in a garbage collecting paradigm.)
 *
 * The reallocated object's "no pointers" flag is maintained between
 *  reallocations. Be careful what you store in it. Newly allocated memory
 *  (by specifying (NULL) for (oldPtr)...) is flagged to potentially contain
 *  pointers, for safety.
 *
 * All other rules of garbage collection apply to this. Don't store pointers
 *  to disk, make sure you NULL out the pointer when done with it, etc...
 *
 *      params : amount == amount of bytes to allocate.
 *     returns : pointer to newly allocated block. Never returns NULL.
 */
{
    void *retVal = GC_realloc(oldPtr, amount);

    if (retVal == NULL)
        __runtimeError(ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memRealloc */


void *__memAllocNoPtrs(__long amount)
/*
 * This is the same as __memAlloc(), except the memory object is flagged
 *  as not meant to contain pointers. This object therefore will not be
 *  checked for references during collections. Of course, this speeds up
 *  the collection process, but might cause premature collection if you
 *  disobey and store a pointer in this. Use this for string data and
 *  the like. Besides setting this "no pointers" flag, this function is
 *  identical to __memAlloc().
 *
 *     params : amount == amount of bytes to allocate.
 *    returns : void.
 */
{
    void *retVal = GC_malloc_atomic(amount);

    if (retVal == NULL)
        __runtimeError(ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memAllocNoPtrs */


void __memFree(void *ptr)
/*
 * Free a pointer allocated by __memAlloc(), __memRealloc(), or
 *  __memAllocNoPtrs().  Do NOT use this on pointers from malloc(), realloc(),
 *  calloc(), or anything else!
 *
 * Actually, this function is completely unnecessary, and probably shouldn't
 *  be used unless you have something really simple to deallocate. Otherwise,
 *  you add extra code to your program, and the potential to prematurely
 *  deallocate an object, which will cause a segmentation fault crash.
 *
 * In short: avoid this.
 *
 * This function just returns if you specify (NULL). This is like free().
 *
 *      params : ptr == memory object to deallocate.
 *     returns : void.
 */
{
    if (ptr != NULL)
        GC_free(ptr);
} /* __memFree */


void __memDoFullCollect(void)
/*
 * Do a full, world-stopping collection. This may take some time, but
 *  frees all possibly memory. The best places for this call are points
 *  where execution can take all day, or when a user can explicitly
 *  command a program to do garbage collection (so it's his own fault. :) )
 *
 * Note that this function is never directly necessary, since the memory
 *  manager will initiate garbage collection when it needs to free some
 *  memory, but it's nice to know you can command it if you know of a
 *  better time to perform collection.
 *
 *      params : void.
 *     returns : void.
 */
{
    if (!GC_dont_gc)
        GC_gcollect();
} /* __memDoFullCollect */


void __memDoPartialCollect(void)
/*
 * Do a little garbage collection. This runs faster than the
 *  world-stopping __memDoFullCollect(), but frees less memory.
 *
 * This is good for idle loops.
 *
 *     params : void.
 *    returns : void.
 */
{
    if (!GC_dont_gc)
        GC_collect_a_little();
} /* __memDoPartialCollect */


void __memEnableGarbageCollector(__boolean shouldEnable)
/*
 * Enable or disable garbage collection. The garbage collector
 *  will NOT run if disabled, even if specifically requested
 *  to do so! This applies to both partial collections and
 *  full, world-stopping collections. Note that enabling the
 *  the garbage collector does not directly instruct it to
 *  collect; it just permits it to do so. To explicitly collect,
 *  call __memDoFullCollect() or __memDoPartialCollect(), or wait
 *  until the memory manager needs to free some memory. If you
 *  disable the garbage collector in the middle of a collection,
 *  that collection will still continue to completion.
 *
 * At startup, garbage collection is enabled by default.
 *
 * This is useful for performance critical sections of code (but remember
 *  that memory swapping is costly, too, so make sure that you aren't
 *  sucking resources when you disable the collector!), but is never
 *  specifically needed. Garbage collection isn't necessarily going to
 *  run during any piece of code.
 *
 *     params : shouldEnable == (true) to allow collection, (false) to prevent.
 *    returns : void.
 */
{
    GC_dont_gc = (shouldEnable ? 0 : 1);
} /* __memEnableGarbageCollector */


__boolean __memIsGarbageCollectorEnabled(void)
/*
 * Determine if garbage collection is enabled.
 *
 *     params : void.
 *    returns : (true) if enabled, (false) otherwise.
 */
{
    return(GC_dont_gc ? false : true);
} /* __memIsGarbageCollectorEnabled */


/* end of MemoryManager.c ... */

