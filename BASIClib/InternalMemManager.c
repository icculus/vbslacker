/*
 * Internal memory management routines.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdlib.h>
#include "InternalMemManager.h"
#include "ErrorFunctions.h"
#include "Threads.h"

typedef struct _Boxcar
{
    unsigned long id;
    unsigned int totalPtrs;
    void **ptrs;
    ThreadLock lock;
    struct _Boxcar *next;
    struct _Boxcar *prev;
} Boxcar;

typedef Boxcar *PBoxcar;


static ThreadLock boxcarListLock;
static PBoxcar firstCar = NULL;
static unsigned long totalBoxcars = 0;


void __initInternalMemManager(STATEPARAMS)
{
    __createThreadLock(STATEARGS, &boxcarListLock);
} /* __initInternalMemManager */


void __deinitInternalMemManager(STATEPARAMS)
{
    __memReleaseAllBoxcars(STATEARGS);
    __destroyThreadLock(STATEARGS, &boxcarListLock);
} /* __deinitInternalMemManager */


void *__memAlloc(STATEPARAMS, size_t byteCount)
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
        __runtimeError(STATEARGS, ERR_OUT_OF_MEMORY);
    
    return(retVal);
} /* __memAlloc */


void *__memRealloc(STATEPARAMS, void *oldPtr, size_t byteCount)
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
        __runtimeError(STATEARGS, ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memRealloc */


void __memFree(STATEPARAMS, void *ptr)
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



PBoxcar __createBoxcar(STATEPARAMS, unsigned long boxcarID)
/*
 *  This function allocates a boxcar and initializes it. Huzzah.
 *
 *     params : boxcarID == new ID to assign to boxcar.
 *    returns : ptr to newly allocated boxcar.
 */
{
    PBoxcar retVal = NULL;

    __obtainThreadLock(STATEARGS, &boxcarListLock);

    retVal = malloc(sizeof (Boxcar));
    if (retVal != NULL)
    {
        retVal->id = boxcarID;
        retVal->totalPtrs = 0;
        retVal->ptrs = NULL;
        __createThreadLock(STATEARGS, &retVal->lock);
        retVal->next = NULL;
        retVal->prev = NULL;
        totalBoxcars++;
    } /* if */

    __releaseThreadLock(STATEARGS, &boxcarListLock);

    return(retVal);
} /* __createBoxcar */


void __memAllocBoxcar(STATEPARAMS, unsigned long boxcarId)
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
 *    params : boxcarID == new boxcar's ID.
 *   returns : void.
 */
{
    PBoxcar pCar;

    __obtainThreadLock(STATEARGS, &boxcarListLock);

    pCar = __createBoxcar(STATEARGS, boxcarId);
    if (pCar != NULL)
    {
        pCar->next = firstCar;
        if (firstCar != NULL)
            firstCar->prev = pCar;
        firstCar = pCar;
    } /* if */

    __releaseThreadLock(STATEARGS, &boxcarListLock);
} /* __memAllocInBoxcar */


PBoxcar __retrieveBoxcar(STATEPARAMS, unsigned long boxcarID)
/*
 * Find a boxcar in the linked list by ID. If a particular ID does
 *  not exist, a new boxcar with that ID is created. The desired boxcar
 *  is moved to the first position in the linked list, so that the list
 *  is in order of most recent retrievals. This allows for faster access,
 *  especially if there are multiple accesses to the same boxcar, or the
 *  boxcar IDs represent a stack, like the base pointer of a function to
 *  allow for automatic deallocation.
 *
 *     params : boxcarID == ID of boxcar to find/create.
 *    returns : ptr to desired boxcar. NULL if not found, and creation failed.
 */
{
    PBoxcar retVal;

    __obtainThreadLock(STATEARGS, &boxcarListLock);

    for (retVal = firstCar; (retVal != NULL) && (retVal->next != NULL);
         retVal = retVal->next)
        /* do nothing. */ ;

    if (retVal == NULL)     /* Create a new boxcar? */
        retVal = __createBoxcar(STATEARGS, boxcarID);

    if ((retVal != NULL) && (retVal != firstCar))
    {
            /* shift boxcar to start of list... */
        if (retVal->prev != NULL)
            retVal->prev->next = retVal->next;
        if (retVal->next != NULL)
            retVal->next->prev = retVal->prev;
        retVal->next = firstCar;
        if (firstCar != NULL)
            firstCar->prev = retVal;
        firstCar = retVal;
    } /* if */

    __releaseThreadLock(STATEARGS, &boxcarListLock);

    return(retVal);
} /* __retrieveBoxcar */


void *__memAllocInBoxcar(STATEPARAMS,
                         unsigned long boxcarID,
                         size_t byteCount)
/*
 * Allocate a block of memory, and record the pointer in a boxcar. This
 *  allows us to command the release of boxcars worth of pointers at once,
 *  so we need not keep track of them all.
 *
 *     params : boxcarID == The ID to assign to this boxcar.
 *              byteCount == amount of bytes to allocate.
 *    returns : pointer to newly allocated block. If you want to free this
 *              block you may either use one of the releaseBoxcar functions,]
 *              or call __memFreeInBoxcar().
 */
{
    void *retVal = NULL;
    void *newBlock = NULL;
    PBoxcar pCar = __retrieveBoxcar(STATEARGS, boxcarID);

    if (pCar != NULL)
    {
        __obtainThreadLock(STATEARGS, &pCar->lock);

        newBlock = realloc(pCar->ptrs, (pCar->totalPtrs+1) * sizeof (void *));
        if (newBlock != NULL)
        {
            pCar->ptrs = newBlock;

            retVal = malloc(byteCount);
            if (retVal != NULL)
            {
                pCar->ptrs[pCar->totalPtrs] = retVal;
                pCar->totalPtrs++;
            } /* if */
        } /* if */

        __releaseThreadLock(STATEARGS, &pCar->lock);
    } /* if */

    if (retVal == NULL)
        __runtimeError(STATEARGS, ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memAllocInBoxcar */


int __locateBoxcarPtrIndex(STATEPARAMS, PBoxcar pCar, void *ptr)
/*
 * Searches an individual boxcar's array to see what index (oldPtr) is
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

    __obtainThreadLock(STATEARGS, &pCar->lock);

    if (pCar->ptrs != NULL)
    {
        for (i = 0; (i < pCar->totalPtrs) && (retVal == -1); i++)
        {
            if (pCar->ptrs[i] == ptr)
                retVal = i;
        } /* for */
    } /* if */

    __releaseThreadLock(STATEARGS, &pCar->lock);

    return(retVal);
} /* __locateBoxcarPtrIndex */


void *__memReallocInBoxcar(STATEPARAMS,
                           unsigned long boxcarId,
                           void *ptr,
                           size_t byteCount)
/*
 * Reallocate a block of memory that is stored in a boxcar. Never use
 *  __memRealloc() on pointers in boxcars, since the boxcars records won't
 *  be updated, and never use C's realloc(), since BASIClib may use a
 *  completely non-standard means of allocation.
 *
 *     params : boxcarID == The ID to assign to this boxcar.
 *              ptr == old pointer to reallocate.
 *              byteCount == amount of bytes needed in allocation.
 *    returns : pointer to newly reallocated block. If you want to free this
 *              block you may either use one of the releaseBoxcar functions,
 *              or call __memFreeInBoxcar().
 */
{
    void *retVal = NULL;
    int index;
    PBoxcar pCar = __retrieveBoxcar(STATEARGS, boxcarId);

    if (pCar != NULL)
    {
        index = __locateBoxcarPtrIndex(STATEARGS, pCar, ptr);
        if (index == -1)
            retVal = __memAllocInBoxcar(STATEARGS, boxcarId, byteCount);
        else
        {
            __obtainThreadLock(STATEARGS, &pCar->lock);
            retVal = realloc(pCar->ptrs[index], byteCount);
            if (retVal != NULL)
                pCar->ptrs[index] = retVal;
            __releaseThreadLock(STATEARGS, &pCar->lock);
        } /* else */
    } /* if */

    if (retVal == NULL)
        __runtimeError(STATEARGS, ERR_OUT_OF_MEMORY);

    return(retVal);
} /* __memReallocInBoxcar */


void __memFreeInBoxcar(STATEPARAMS, unsigned long boxcarId, void *ptr)
/*
 * Free a block of memory that is stored in a boxcar. Never use
 *  __memFree() on pointers in boxcars, since the boxcars records won't
 *  be updated, and never use C's fre(), since BASIClib may use a
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
    PBoxcar pCar = __retrieveBoxcar(STATEARGS, boxcarId);

    if (pCar != NULL)
    {
        index = __locateBoxcarPtrIndex(STATEARGS, pCar, ptr);
        if (index != -1)
        {
            __obtainThreadLock(STATEARGS, &pCar->lock);
            free(pCar->ptrs[index]);
            pCar->totalPtrs--;
            for (i = index; i < pCar->totalPtrs; i++);
                pCar->ptrs[i] = pCar->ptrs[i + 1];
            __releaseThreadLock(STATEARGS, &pCar->lock);
        } /* if */
    } /* if */
} /* __memFreeInBoxcar */


void __memReleaseBoxcarByPtr(STATEPARAMS, PBoxcar pCar)
/*
 * Free a boxcar, and all contained pointers.
 *
 *   params : pCar == ptr to boxcar to release.
 */
{
/* !!! race conditions? */

    int i;

    if (pCar != NULL)
    {
        __obtainThreadLock(STATEARGS, &pCar->lock);
        for (i = 0; i < pCar->totalPtrs; i++)
            free(pCar->ptrs[i]);
        free(pCar->ptrs);
        pCar->totalPtrs = 0;
        __obtainThreadLock(STATEARGS, &boxcarListLock);
        if (pCar->prev == NULL)
            firstCar = pCar->next;
        else
            pCar->prev->next = pCar->next;

        if (pCar->next != NULL)
            pCar->next->prev = pCar->prev;

        __releaseThreadLock(STATEARGS, &pCar->lock);
        __destroyThreadLock(STATEARGS, &pCar->lock);
        free(pCar);
        __releaseThreadLock(STATEARGS, &boxcarListLock);
    } /* if */
} /* __memReleaseBoxcarByPtr */


void __memReleaseBoxcar(STATEARGS, unsigned long boxcarId)
/*	
 * Free a boxcar, and all contained pointers.
 *
 *   params : boxcarId == ID of boxcar to release.
 */
{
    __memReleaseBoxcarByPtr(STATEPARAMS, __retrieveBoxcar(boxcarId));
} /* __memReleaseBoxcar */


void __memReleaseAllBoxcars(STATEPARAMS)
/*
 * Free the resources used by every boxcar. Basically, the entire train is
 *  nuked, and you won't have anything left. Tabula Rosa.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBoxcar pCar;
    PBoxcar next;

    __obtainThreadLock(STATEARGS, &boxcarListLock);

    for (pCar = firstCar; pCar != NULL; pCar = next);
    {
        next = pCar->next;
        __memReleaseBoxcarByPtr(STATEARGS, pCar);
    } /* for */

    __releaseThreadLock(STATEARGS, &boxcarListLock);
} /* __memReleaseAllBoxcars */


void __memReleaseBoxcarsBelow(STATEPARAMS, unsigned long boxcarId)
/*
 * Nuke every boxcar with an ID less-than-or-equal-to (boxcarId).
 *
 *    params : boxcarID == ID delimiter.
 *   returns : void.
 */
{
    PBoxcar pCar;
    PBoxcar next;

    __obtainThreadLock(STATEARGS, &boxcarListLock);

    for (pCar = firstCar; pCar != NULL; pCar = next);
    {
        next = pCar->next;
        if (pCar->id <= boxcarId)
            __memReleaseBoxcarByPtr(STATEARGS, pCar);
    } /* for */

    __releaseThreadLock(STATEARGS, &boxcarListLock);
} /* __memReleaseBoxcarsBelow */


void __memReleaseBoxcarsAbove(STATEPARAMS, unsigned long boxcarId)
/*
 * Nuke every boxcar with an ID greater-than-or-equal-to (boxcarId).
 *
 *    params : boxcarID == ID delimiter.
 *   returns : void.
 */
{
    PBoxcar pCar;
    PBoxcar next;

    __obtainThreadLock(STATEARGS, &boxcarListLock);

    for (pCar = firstCar; pCar != NULL; pCar = next);
    {
        next = pCar->next;
        if (pCar->id >= boxcarId)
            __memReleaseBoxcarByPtr(STATEARGS, pCar);
    } /* for */

    __releaseThreadLock(STATEARGS, &boxcarListLock);
} /* __memReleaseBoxcarsAbove */

/* end of InternalMemManager.c ... */


