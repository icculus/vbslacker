/*
 * Memory management routines.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdlib.h>
#include "MemoryManager.h"


    /*
     * ABOVESTACK is a comparison operator based on which
     *  direction the stack grows on a given platform...
     */
#if (STACK_DIRECTION == -1)
#    define ABOVESTACK <=
#else
#    define ABOVESTACK >=
#endif

#define SLACKCOLLECT "SLACKCOLLECT"

typedef struct _Boxcar
{
    void *id;
    unsigned int totalPtrs;
    void **ptrs;
    struct _Boxcar *next;
    struct _Boxcar *prev;
} Boxcar;

typedef Boxcar *PBoxcar;


static int collectionMax = 20;       /* override by env. var... */
static PBoxcar *trains = NULL;
static ThreadLock trainLock = NULL;


void __initMemoryManager(STATEPARAMS)
/*
 * Create the memory manager's ThreadLock, and checks the SLACKCOLLECT
 *  environment variable.
 *
 *    params : void.
 *   returns : void.
 *
 */
{
    char *rc;

    __createThreadLock(STATEARGS, &trainLock);

    rc = getenv(SLACKCOLLECT);
    if (rc != NULL)
        collectionMax = atoi(rc);
} /* __initMemoryManager */


void __deinitMemoryManager(STATEPARAMS)
{
    __destroyThreadLock(STATEARGS, &trainLock);
} /* __deinitMemoryManager */


void __initThreadMemoryManager(STATEPARAMS, int tidx)
{
    void *rc;

    rc = malloc(__getHighestThreadIndex(STATEARGS) * sizeof (PBoxcar));
    if (rc == NULL)
        __fatalRuntimeError(STATEARGS, ERR_OUT_OF_MEMORY);
    else
    {
        __obtainThreadLock(STATEARGS, &trainLock);
        free(trains);
        trains = rc;
        __releaseThreadLock(STATEARGS, &trainLock);
    } /* else */
} /* __initThreadMemoryManager */


void __deinitThreadMemoryManager(STATEARGS, int tidx)
{
    __memReleaseAllBoxcars(STATEARGS);
} /* __deinitThreadMemoryManager */


static void __setStartOfTrain(STATEPARAMS, PBoxcar newStart)
{
    __obtainThreadLock(STATEARGS, &trainLock);
    trains[__getCurrentThreadIndex(STATEARGS)] = retVal;
    __releaseThreadLock(STATEARGS, &trainLock);
} /* __setStartOfTrain */


static PBoxcar __getStartOfTrain(STATEPARAMS)
{
    PBoxcar retVal;

    __obtainThreadLock(STATEARGS, &trainLock);
    retVal = trains[__getCurrentThreadIndex(STATEARGS)];
    __releaseThreadLock(STATEARGS, &trainLock);

    return(retVal);
} /* __getTrainCurrentThread */


void *__memAlloc(STATEPARAMS, size_t byteCount)
/*
 * Allocate a block of memory. Works like malloc(), except it throws
 *  a runtime error if there isn't enough memory for the allocation.
 *  Therefore this function never returns a NULL pointer.
 *
 *  (Actually, it's not entirely like malloc(), since we try to
 *   free some boxcars and allocate again if the call fails the first
 *   time...)
 *
 *     params : byteCount == number of bytes to allocate.
 *    returns : pointer to newly allocated memory.
 */
{
    __boolean boxcarsRemain;
    void *retVal;

    do
    {
        retVal = malloc(byteCount);
        if (retVal == NULL)
        {
            boxcarsRemain = __forcePartialMemCleanup(STATEARGS);
            retVal = malloc(byteCount);
        } /* if */
    } while ((retVal == NULL) && (boxcarsRemain == true));

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
 *  (Well, it's not ENTIRELY like realloc(), since we try to
 *   free some boxcars and allocate again if the call fails the first
 *   time...)
 *
 *     params : oldPtr == pointer to reallocate. NULL allocates a new block.
 *              byteCount == number of bytes to allocate.
 *    returns : pointer to newly allocated memory.
 */
{
    __boolean boxcarsRemain;
    void *retVal;

    do
    {
        retVal = realloc(oldPtr, byteCount);
        if (retVal == NULL)
        {
            boxcarsRemain = __forcePartialMemCleanup(STATEARGS);
            retVal = realloc(oldPtr, byteCount);
        } /* if */
    } while ((retVal == NULL) && (boxcarsRemain == true));

    if (retVal == NULL)
        __runtimeError(STATEARGS, ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memRealloc */


void __memFree(STATEPARAMS, void *ptr)
/*
 * Like free(), but only use this with pointers returned from
 *  __memAlloc() or __memRealloc()...
 *
 *    params : ptr == pointer to block of memory to free.
 *   returns : void.
 */
{
    free(ptr);
} /* __memFree */


#if 0
void __memAllocBoxcar(STATEPARAMS)
/*
 * This function creates a new boxcar, and sticks it at the start of the
 *  list. This is for efficiency in programs that definitely know when to
 *  build a new boxcar, so the entire linked list need not be searched to
 *  verify an ID does not exist before building, as __retrieveBoxcar() does.
 *
 * If you call this function, it MUST be done before you call any other
 *  boxcar functions on a given ID. Otherwise, there'll be two boxcars with
 *  identical IDs, and the first one will stop being used; pointers will
 *  probably be lost.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBoxcar pCar;


    pCar = __createBoxcar(STATEARGS);
    if (pCar != NULL)
    {
        __obtainThreadLock(STATEARGS, &boxcarListLock);
        pCar->next = firstCar;
        if (firstCar != NULL)
            firstCar->prev = pCar;
        firstCar = pCar;
        __releaseThreadLock(STATEARGS, &boxcarListLock);
    } /* if */
} /* __memAllocBoxcar */
#endif


static PBoxcar __retrieveBoxcar(STATEPARAMS)
/*
 * Find a boxcar in the linked list by ID. If a particular ID does
 *  not exist, a new boxcar with that ID is created. The desired boxcar
 *  is moved to the first position in the linked list, so that the list
 *  is in order of most recent retrievals. This allows for faster access,
 *  especially if there are multiple accesses to the same boxcar, or the
 *  boxcar IDs represent a stack, like the base pointer of a function to
 *  allow for automatic deallocation.
 * !!! reexamine this comment.
 *     params : boxcarID == ID of boxcar to find/create.
 *    returns : ptr to desired boxcar. NULL if not found, and creation failed.
 */
{
    PBoxcar firstCar = __getStartOfTrain(STATEARGS);
    PBoxcar retVal;

    for (retVal = firstCar;
         ((retVal != NULL) && (retVal->id != __stBP);
         retVal = retVal->next)
            /* do nothing. */ ;

    if (retVal == NULL)     /* Create a new boxcar? */
    {
        PBoxcar retVal = __memAlloc(STATEARGS, sizeof (Boxcar));
        retVal->id = __stBP;   /* part of STATEPARAMS ... */
        retVal->totalPtrs = 0;
        retVal->ptrs = NULL;
        retVal->next = NULL;
        retVal->prev = NULL;
    } /* if */

    if (retVal != firstCar)
    {
            /* shift boxcar to start of list... */
        if (retVal->prev != NULL)
            retVal->prev->next = retVal->next;
        if (retVal->next != NULL)
            retVal->next->prev = retVal->prev;
        retVal->next = firstCar;
        retVal->prev = NULL;
        if (firstCar != NULL)
            firstCar->prev = retVal;
        __setStartOfTrain(STATEARGS, retVal);
    } /* if */

                  /* !!! comment that crap better... */
    return(retVal);
} /* __retrieveBoxcar */


void *__memAllocInBoxcar(STATEPARAMS, size_t byteCount)
/*
 * Allocate a block of memory, and record the pointer in a boxcar. This
 *  allows us to command the release of boxcars worth of pointers at once,
 *  so we need not keep track of them all.
 *
 *     params : byteCount == amount of bytes to allocate.
 *    returns : pointer to newly allocated object. If you want to free this
 *              object you may either use the __releaseBoxcar() function,
 *              or call __memFreeInBoxcar().
 */
{
    void *retVal = NULL;
    PBoxcar pCar = __retrieveBoxcar(STATEARGS);

    pCar->ptrs = __memRealloc(STATEARGS, pCar->ptrs,
                              (pCar->totalPtrs + 1) * sizeof (void *));

    retVal = pCar->ptrs[pCar->totalPtrs] = __memAlloc(STATEARGS, byteCount);
    pCar->totalPtrs++;

    return(retVal);
} /* __memAllocInBoxcar */


static int __locateBoxcarPtrIndex(STATEPARAMS, PBoxcar pCar, void *ptr)
/*
 * Searches an individual boxcar's array to see what index (ptr) is
 *  stored at.
 *
 *    params : pCar == ptr to Boxcar to check.
 *             ptr == pointer to find.
 *   returns : index, or -1 if not found.
 */
{
#warning __locateBoxcarPtrIndex() could use a more efficient search algorithm!

    int i;
    int retVal = -1;

    for (i = 0; (i < pCar->totalPtrs) && (retVal == -1); i++)
    {
        if (pCar->ptrs[i] == ptr)
            retVal = i;
    } /* for */

    return(retVal);
} /* __locateBoxcarPtrIndex */


void *__memReallocInBoxcar(STATEPARAMS, void *ptr, int byteCount)
/*
 * Reallocate a block of memory that is stored in a boxcar. Never use
 *  __memRealloc() on pointers in boxcars, since the boxcars records won't
 *  be updated, and never use C's realloc(), since BASIClib may use a
 *  completely non-standard means of allocation.
 *
 *     params : ptr == old pointer to reallocate.
 *              byteCount == amount of bytes needed in allocation.
 *    returns : pointer to newly reallocated block. If you want to free this
 *              block you may either use one of the releaseBoxcar functions,
 *              or call __memFreeInBoxcar().
 */
{
    void *retVal = NULL;
    int index;
    PBoxcar pCar = __retrieveBoxcar(STATEARGS);

    index = __locateBoxcarPtrIndex(STATEARGS, pCar, ptr);
    if (index == -1)
        retVal = __memAllocInBoxcar(STATEARGS, byteCount);
    else
    {
        retVal = pCar->ptrs[index] = __memRealloc(STATEARGS,
                                                  pCar->ptrs[index],
                                                  byteCount);
    } /* else */

    return(retVal);
} /* __memReallocInBoxcar */


void __memFreeInBoxcar(STATEPARAMS, void *ptr)
/*
 * Free a block of memory that is stored in a boxcar. Never use
 *  __memFree() on pointers in boxcars, since the boxcars records won't
 *  be updated, and never use C's free(), since BASIClib may use a
 *  completely non-standard means of allocation.
 *
 *  (Calling this with a bogus ID will still create a boxcar, even though
 *   the intention is to free memory.)
 *
 *     params : boxcarID == The ID to assign to this boxcar.
 *    returns : void.
 */
{
    int index;
    int i;
    PBoxcar pCar;

    if (ptr != NULL)
    {
        pCar = __retrieveBoxcar(STATEARGS);

        index = __locateBoxcarPtrIndex(STATEARGS, pCar, ptr);
        if (index != -1)
        {
            free(pCar->ptrs[index]);
            pCar->totalPtrs--;
            for (i = index; i < pCar->totalPtrs; i++);
                pCar->ptrs[i] = pCar->ptrs[i + 1];
        } /* if */
    } /* if */
} /* __memFreeInBoxcar */


static int __releaseBoxcar(STATEPARAMS, PBoxcar pCar)
/*
 * Free a boxcar, and all contained pointers.
 *
 *   params : pCar == ptr to boxcar to release.
 *  returns : number of ptrs contained in boxcar before release.
 */
{
    int i;

    for (i = 0; i < pCar->totalPtrs; i++)
        __memFree(pCar->ptrs[i]);
    __memFree(pCar->ptrs);

    if (pCar->prev == NULL)
        __setStartOfTrain(STATEARGS, pCar->next);
    else
        pCar->prev->next = pCar->next;

    if (pCar->next != NULL)
        pCar->next->prev = pCar->prev;

    __memFree(pCar);
} /* __memReleaseBoxcar */


void __memForceFullBoxcarRelease(STATEPARAMS)
/*
 * Force the "garbage collector" to reclaim every possible bit of
 *  memory. Could potentially take awhile to run, and blocks until
 *  complete. Returns immediately without any cleanup if SLACKCOLLECT
 *  environment variable is set <= 0. In short, big memory leaks, since
 *  garbage collection is disabled.
 *
 *     params : void.
 *    returns : void.
 */
{
    PBoxcar pCar;
    PBoxcar next;
    void *base_ptr;

    if (collectionMax > 0)
    {
        __getBasePointer(&base_ptr);

        for (pCar = __getStartOfTrain(STATEARGS); pCar != NULL; pCar = next)
        {
            next = pCar->next;
            if (pCar->id ABOVESTACK base_ptr)
                __memReleaseBoxcar(STATEARGS, pCar);
        } /* for */
    } /* if */
} /* __memForceFullBoxcarRelease */


__boolean __memForcePartialBoxcarRelease(STATEPARAMS)
/*
 * Force the "garbage collector" to release whatever boxcars can be
 *  cleaned up in a relatively timely manner. Good to call this in
 *  an idle loop, so collection is done a little at a time until
 *  either the loop exits, or all garbage has been collected.
 *
 * For each call, up to 20 pointers are freed, unless the SLACKCOLLECT
 *  environment variable has been set, in which case the pointer count
 *  is taken from there. A number <= 0 in SLACKCOLLECT disables
 *  garbage collection, and effectively enables massive memory leaking.
 *
 *     params : void.
 *    returns : boolean (true) if garbage remains, boolean (false) otherwise.
 */
{
#warning Partial collections can exceed SLACKCOLLECT.
    PBoxcar next;
    void *base_ptr;
    int ptrCount = 0;
    PBoxcar pCar = (PBoxcar) __getStartOfTrain(STATEARGS);

    __getBasePointer(&base_ptr);

    while ((pCar != NULL) &&
           (ptrCount < collectionMax) &&
           (pCar->id ABOVESTACK base_ptr))
    {
        next = pCar->next;
        ptrCount += __memReleaseBoxcar(STATEARGS, pCar);
        pCar = next;
    } /* while */

    return((pCar == NULL) ? false : true);
} /* __memForcePartialBoxcarRelease */


static void __memReleaseAllBoxcars(STATEPARAMS)
/*
 * Free all boxcars in this thread, regardless of whether they're in
 *  use or not. Dangerous, so we made it static. This is only called
 *  from __deinitThreadMemoryManager() right now, so no leaks are
 *  left when threads terminate.
 *
 *      params : void.
 *     returns : void.
 */
{
    PBoxcar next;
    PBoxcar pCar;

    if (collectionMax > 0)
    {
        for (pCar = __getStartOfTrain(STATEARGS); pCar != NULL; pCar = next)
        {
            next = pCar->next;
            __memReleaseBoxcar(STATEARGS, pCar);
        } /* for */
    } /* if */
} /* __memReleaseAllBoxcars */

/* end of MemoryManager.c ... */

