/*
 * Memory management routines.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MemoryManager.h"

#warning COMMENT THE MEMORYMANAGER!!!


    /*
     * ABOVESTACK is a comparison operator based on which
     *  direction the stack grows on a given platform...
     * STACKINCREMENT is a macro that adds (or subtracts)
     *  in the way a pointer must move to go towards the
     *  the top of the stack.
     */
#if (STACK_DIRECTION == -1)
#    define ABOVESTACK <=
#    define STACKINCREMENT(x) ((x)--)
#else
#    define ABOVESTACK >=
#    define STACKINCREMENT(x) ((x)++)
#endif

    /*
     * SLACKCOLLECT is the name of the environment variable
     *  that defines the max pointer freeing count for partial boxcar
     *  releases.
     */
#define SLACKCOLLECT     "SLACKCOLLECT"

    /*
     * ALLOCFLOODLEVEL is the count of allocated pointers before a
     *  boxcar flood is suspected.
     */
#define ALLOCFLOODLEVEL  30


    /*
     * This is the information for one memory object in a boxcar.
     *  (ptr) stores the actual pointer to the allocated memory.
     *  (isGarbage) signifies whether the stack scanner has
     *   declared this memory object as unreferenced.
     *  (finalizer) is for future C++ support.
     */
typedef struct
{
    void *ptr;
    __boolean isGarbage;
    void (*finalizer)(STATEPARAMS);
} MemoryObject;

typedef MemoryObject *PMemoryObject;


typedef struct
{
    void *id;
    unsigned int totalObjs;
    PMemoryObject objs;
    struct _Boxcar *next;
    struct _Boxcar *prev;
    unsigned int overflowCount;
} Boxcar;

typedef Boxcar *PBoxcar;


static int collectionMax = 20;       /* override by env. var... */
static PBoxcar *trains = NULL;
static ThreadLock trainLock;


static void __memReleaseAllBoxcars(STATEPARAMS);


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
/*
 * Deinitialize; destroys the ThreadLock.
 *
 *   params : void.
 *  returns : void.
 */
{
    __destroyThreadLock(STATEARGS, &trainLock);
} /* __deinitMemoryManager */


void __initThreadMemoryManager(STATEPARAMS, int tidx)
/*
 * Each thread has its own memory "train," which is a linked list
 *  of boxcars. This is to keep the thread-proofing simple and efficient,
 *  since the ThreadLock only need be kept long enough to find the first
 *  car in the train: that is, to figure out which Thread Index we are in,
 *  and access that element of an array. This routine makes sure that array
 *  is set up correctly for the newly-spun thread.
 *
 * This routine will raise a fatal runtime error if realloc() fails.
 *
 *     params : tidx == Thread index of dying thread.
 *    returns : void.
 */
{
    __obtainThreadLock(STATEARGS, &trainLock);
    trains = realloc(trains,
                     __getHighestThreadIndex(STATEARGS) * sizeof(PBoxcar));
    if (trains == NULL)
        __fatalRuntimeError(STATEARGS, ERR_OUT_OF_MEMORY);
    trains[tidx] = NULL;
    __releaseThreadLock(STATEARGS, &trainLock);
} /* __initThreadMemoryManager */


void __deinitThreadMemoryManager(STATEPARAMS, int tidx)
/*
 * Thread is terminating; release all of its boxcars with extreme prejudice.
 *
 *     params : tidx == Thread index of dying thread.
 *    returns : void.
 */
{
    __memReleaseAllBoxcars(STATEARGS);
} /* __deinitThreadMemoryManager */


static void __setStartOfTrain(STATEPARAMS, PBoxcar newStart)
/*
 * Set the start of the thread's memory train. New cars are added to
 *  the front, since they are most likely to be accessed.
 *
 *    params : newStart == new beginning of train.
 *   returns : void.
 */
{
    __obtainThreadLock(STATEARGS, &trainLock);
    trains[__getCurrentThreadIndex(STATEARGS)] = newStart;
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
            boxcarsRemain = __memForcePartialBoxcarRelease(STATEARGS);
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
            boxcarsRemain = __memForcePartialBoxcarRelease(STATEARGS);
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
         ((retVal != NULL) && (retVal->id != __stBP));
         retVal = retVal->next)
            /* do nothing. */ ;

    if (retVal == NULL)     /* Create a new boxcar? */
    {
        PBoxcar retVal = __memAlloc(STATEARGS, sizeof (Boxcar));
        memset(retVal, '\0', sizeof (Boxcar));
        retVal->id = __stBP;   /* part of STATEPARAMS ... */
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


static void __scanForLocalGarbage(STATEPARAMS, PBoxcar pCar)
/*
 * This is a simple, conservative garbage collector. Basically,
 *  it'll scan the stack from (pCar->id) (the base pointer of (pCar)'s
 *  associated function) to the current base pointer. This should be
 *  any local variables (and some other minor stack noise) in a given
 *  function. If no reference to a given pointer in (pCar) is found,
 *  we consider it garbage, and collect it.
 *
 *    params : pCar == boxcar to use for scan.
 *   returns : void.
 */
{
    void *stackLoc;
    void *basePtr;
    int i;

#ifdef DEBUG
    unsigned int objsFreed = 0;
    printf("Conservative Garbage Collector starting...\n");
    printf("  - Checking at most %d bytes of stack.\n", MAXSTACKSCAN);
    printf("  - There are %d memory objects to free\n", pCar->totalPtrs);
#endif

    __getBasePointer(&basePtr);

    memset(flags, '\0', pCar->totalPtrs * sizeof (__boolean));

    for (stackLoc = pCar->id; basePtr ABOVESTACK stackLoc; stackLoc++)
    {
        for (i = 0; i < pCar->totalPtrs; i++)
        {
            if ( pCar->ptrs[i] == *((void **) stackLoc) )
                flags[i] = true;
        } /* for */
    } /* for */

    for (i = 0; i < pCar->totalPtrs; i++)
    {
        if (flags[i] != true)   /* !!! won't work. */
            __memFreeInBoxcar(STATEARGS, pCar->ptrs[i]);
#ifdef DEBUG
        objsFreed++;
#endif    
    } /* for */

#ifdef DEBUG
    printf("Done. Collected %d objects.\n", objsFreed);
#endif    

} /* __scanForLocalGarbage */


static void __incrementBoxcarPointers(STATEPARAMS, PBoxcar pCar)
/*
 * Increment the pointer counters for a boxcar. If the overflow
 *  count passes ALLOCFLOODLEVEL, run the collector (yikes) on a
 *  bit of the stack.
 *
 *     params : pCar == Boxcar to increment.
 *    returns : void.
 */
{
    pCar->totalPtrs++;
    pCar->overflowCount++;
    if (pCar->overflowCount > ALLOCFLOODLEVEL)
    {
        __scanForLocalGarbage(STATEARGS, pCar);
        pCar->overflowCount = 0;
    } /* if */
} /* __incrementBoxcarPointers */


static void __decrementBoxcarPointers(STATEPARAMS, PBoxcar pCar)
/*
 * Decrement the pointer counters for a boxcar. This is split off
 *  into a function to coincide with __incrementBoxcarPointers().
 *
 *     params : pCar == Boxcar to decrement.
 *    returns : void.
 */
{
    pCar->totalPtrs--;
    pCar->overflowCount--;
} /* __decrementBoxcarPointers */


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

    __incrementBoxcarPointers(STATEARGS, pCar);

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
    {
        fprintf(stderr, "__memReallocInBoxcar() on non-existant pointer!\n");
        retVal = __memAllocInBoxcar(STATEARGS, byteCount);
    } /* if */
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


static int __memReleaseBoxcar(STATEPARAMS, PBoxcar pCar)
/*
 * Free a boxcar, and all contained pointers.
 *
 *   params : pCar == ptr to boxcar to release.
 *  returns : number of ptrs contained in boxcar before release.
 */
{
    int retVal = pCar->totalPtrs;
    int i;

    for (i = 0; i < retVal; i++)
        __memFree(STATEARGS, pCar->ptrs[i]);

    __memFree(STATEARGS, pCar->ptrs);

    if (pCar->prev == NULL)
        __setStartOfTrain(STATEARGS, pCar->next);
    else
        pCar->prev->next = pCar->next;

    if (pCar->next != NULL)
        pCar->next->prev = pCar->prev;

    __memFree(STATEARGS, pCar);

    return(retVal);
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

