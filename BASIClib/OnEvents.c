/*
 * ON EVENT GOTO support routines.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdlib.h>
#include "OnEvents.h"
#include "Initialize.h"
#include "ErrorFunctions.h"
#include "InternalMemManager.h"
#include "Threads.h"
#include "Boolean.h"


#ifdef WIN32
    #define CLEANUPPROC "___cleanupOnEventHandler"
#else
    #define CLEANUPPROC "__cleanupOnEventHandler"
#endif

#warning is ERR_INTERNAL_ERROR good for malloc() fails in OnEvents?


    /*
     * Internal structures for keeping track of registered event handlers
     *  and other state information. There is one OnEventsState structure 
     *  for each thread.
     */

typedef struct
{
    OnEventTypeEnum evType;
    void *basePtr;
    void *stackPtr;
    void *protectedStack;
    unsigned long protStackSize;
    void *retAddr;
} OnEventHandlerPtrs;

typedef OnEventHandlerPtrs *POnEventHandlerPtrs;

typedef struct
{
    POnEventHandler *handlers;
    unsigned long handlerCount;
    POnEventHandlerPtrs ptrs;
    unsigned long ptrCount;
} OnEventsState;

typedef OnEventsState *POnEventsState;


    /*
     * module scope thread lock.
     */
ThreadLock onEventsLock;

    /*
     * This is used in __initThreadOnEvents(), to determine whether we need
     *  more memory allocated to handle new threads.
     */
static unsigned int allocatedThreads = 0;

    /*
     * ppState stores the OnEventsState structures used by each thread. This is
     *  an array of pointers, so we only have to use thread locks when accessing
     *  this dynamic array, and not the individual structures.
     */
static POnEventsState *ppState = NULL;


void __initOnEvents(STATEPARAMS)
/*
 * This is called once at program startup.
 *  We use this to create our thread locks.
 *
 *    params : void.
 *   returns : void.
 */
{
    __createThreadLock(STATEARGS, &onEventsLock);
} /* __initOnEvents */


void __deinitOnEvents(STATEPARAMS)
/*
 * This is called once at program termination.
 *  We use this to destroy our thread locks.
 *
 *    params : void.
 *   returns : void.
 */
{
    __destroyThreadLock(STATEARGS, &onEventsLock);
} /* __deinitOnEvents */


POnEventsState __getOnEventsState(STATEPARAMS)    
/*
 * Get a pointer to current thread's OnEventsState structure. This
 *  takes care of all the thread-proofing details.
 *
 *     params : void.
 *    returns : Pointer to current OnEventsState.
 */
{
    POnEventsState retVal;

    __obtainThreadLock(STATEARGS, &onEventsLock);
    retVal = ppState[__getCurrentThreadIndex(STATEARGS)];
    __releaseThreadLock(STATEARGS, &onEventsLock);

    return(retVal);
} /* __getOnEventsState */


void __initThreadOnEvents(STATEPARAMS, int tidx)
/*
 * This makes sure space exists in the thread-protected arrays for the
 *  current thread index. This is called whenever a new thread is created.
 *
 *      params : void.
 *     returns : void, but all the tables could get __memRealloc()ed.
 */
{
    int currentThreadCount;
    POnEventsState pState;

    pState = malloc(sizeof (OnEventsState));
    if (pState == NULL)
        __fatalRuntimeError(STATEARGS, ERR_INTERNAL_ERROR);

    pState->handlers = NULL;
    pState->handlerCount = 0;
    pState->ptrs = NULL;
    pState->ptrCount = 0;

    currentThreadCount = __getHighestThreadIndex(STATEARGS) + 1;

    __obtainThreadLock(STATEARGS, &onEventsLock);

    if (allocatedThreads < currentThreadCount)
    {
        allocatedThreads = currentThreadCount;
        ppState = realloc(ppState, currentThreadCount * sizeof (POnEventsState));
        if (ppState == NULL)
            __fatalRuntimeError(STATEARGS, ERR_INTERNAL_ERROR);
    } /* if */

    ppState[tidx] = pState;

    __releaseThreadLock(STATEARGS, &onEventsLock);
} /* __initThreadOnEvents */


void __deinitThreadOnEvents(STATEPARAMS, int tidx)
/*
 * A thread has died; free its resources.
 *
 *    params : tidx == dead thread's index.
 *   returns : void.
 */
{
    POnEventsState pState;
    unsigned int i;

    __obtainThreadLock(STATEARGS, &onEventsLock);
    pState = ppState[tidx];    
    ppState[tidx] = NULL;
    __releaseThreadLock(STATEARGS, &onEventsLock);

    __deregisterAllOnEventHandlers(STATEARGS);
    free(pState->handlers);

    for (i = 0; i < pState->ptrCount; i++)
        free(pState->ptrs[i].protectedStack);

    free(pState->ptrs);
    free(pState);
    ppState[tidx] = NULL;
} /* __deinitThreadOnEvents */


POnEventHandler __getOnEventHandler(STATEPARAMS, OnEventTypeEnum evType)
/*
 * Returns the current ON EVENT GOTO handler for event type evType.
 *
 *    params : evType == type of event we want handler for.
 *   returns : most significant handler for given event type. NULL if
 *              there isn't one.
 */
{
    POnEventHandler retVal = NULL;
    POnEventHandler *vect;
    POnEventsState pState = __getOnEventsState(STATEARGS);
    unsigned int i;

    for (i = pState->handlerCount - 1, vect = pState->handlers;
        (i >= 0) && (retVal == NULL); i--)
    {
        if (vect[i]->evType == evType)
            retVal = vect[i];
    } /* for */

    return(retVal);
} /* __getOnEventHandler */


void __registerOnEventHandler(STATEPARAMS, void *handlerAddr,
                              void *basePtr, void *stackPtr,
                              OnEventTypeEnum evType)
/*
 * This is somewhere between "low level but general" and "80x386 specific."
 *
 * Ideally, any module that contains a call to this function should
 *  do something like this:
 *
 *   __getStackPointer(&_stack_ptr_);
 *   __getBasePointer(&_base_ptr);
 *   __registerOnEventHandler(&&handlerLabel, _base_ptr_, _stack_ptr_, ONERROR);
 *
 * __getStackPointer() is an inline assembly macro that simply gives up the
 *  current stack pointer. We need this to be inlined, and done before the call
 *  to this function begins, so we have a stable stack pointer.
 *
 * Same thing applies to _base_ptr_.
 *
 * handlerAddr is the goto label we'll be blindly jumping to to handle the
 *  the runtime error.
 *
 * evType just guarantees that we don't call a timer handler for a runtime
 *  error, etc...
 *
 *    (*whew!*)
 *
 * All that said, call this function to register an event handler with
 *  BASIClib that will be called when appropriate. Ideally, the parser/compiler
 *  will generate all the calls to this function.
 *
 * Only one event handler of each type can be active for any given procedure.
 *  This function will check the handler to see if it should replace a
 *  previous one, based on the basePtr argument, which will be constant for all
 *  calls from an instance of a single procedure.
 *
 *     returns : void.
 */
{
    POnEventHandler pHandler = NULL;
    POnEventsState pState = __getOnEventsState(STATEARGS);
    int i;
    boolean getOut = false;



    /* mom's : 215-881-5341 */

        /* determine if we should replace a currently registered handler. */
    for (i = pState->handlerCount - 1; (i <= 0) && (getOut == false); i--)
    {
        if (pState->handlers[i]->basePtr != basePtr)
            getOut = true;
        else if (pState->handlers[i]->evType == evType)
        {
            getOut = true;
            pHandler = pState->handlers[i];
        } /* else if */
    } /* for */

    if (pHandler == NULL)   /* no previously registered handler? */
    {
        pHandler = malloc(sizeof (OnEventHandler));

            /* allocate space in array for new handler. */
        pState->handlerCount++;
        pState->handlers = realloc(pState->handlers, pState->handlerCount *
                                                     sizeof (POnEventHandler));

        if ((pState->handlers == NULL) || (pHandler == NULL))
            __fatalRuntimeError(STATEARGS, ERR_INTERNAL_ERROR);
        else
            pState->handlers[pState->handlerCount - 1] = pHandler;
    } /* if */

    pHandler->handlerAddr = handlerAddr;
    pHandler->stackPtr = stackPtr;
    pHandler->basePtr = basePtr;
    pHandler->evType = evType;
} /* __registerOnEventHandler */


void __deregisterAllOnEventHandlers(STATEPARAMS)
/*
 * This function does the same thing as __deregisterOnEventHandlers(), but
 *  considers all OnEvent handlers irrelevant, thus removing them all.
 *
 *     params : void.
 *    returns : void.
 */
{
    int i;
    POnEventsState pState = __getOnEventsState(STATEARGS);

    for (i = pState->handlerCount; i > 0; i--) 
        free(pState->handlers[i]);

    free(pState->handlers);
    pState->handlers = NULL;
    pState->handlerCount = 0;
} /* __deregisterAllOnEventHandlers */


void __deregisterOnEventHandlers(STATEPARAMS)
/*
 * Call this to deregister irrelevant event handlers. Calls to this will
 *  be automatically generated by the parser/compiler when needed, for
 *  commands such as EXIT SUB. All handlers past the end of the current
 *  stack are assumed to be useless, and are deregistered.
 *
 *      params : void.
 *     returns : void.
 */
{
    POnEventsState pState = __getOnEventsState(STATEARGS);
    POnEventHandler *ppHandler;
    void *ebp;
    int i;

    __getBasePointer(&ebp);

    for (i = pState->handlerCount;
            (i >= 0) && (pState->handlers[i]->basePtr <= ebp);
            i--)
    {
        pState->handlerCount--;
        free(pState->handlers[i]);
    } /* for */

    ppHandler = realloc(pState->handlers,
                             pState->handlerCount * (sizeof (POnEventHandler)));

    if (ppHandler != NULL)
        pState->handlers = ppHandler;
} /* __deregisterOnEventHandler */


void __triggerOnEventByType(STATEPARAMS, OnEventTypeEnum evType)
/*
 * This function calls an OnEvent handler, by figuring out the last
 *  handler registered for (evType) type events.
 *
 *    params : evType == Event type to handle.
 *   returns : either won't return directly, due to stack voodoo, or in the
 *              case of a RESUME NEXT, void.
 */
{
    __triggerOnEvent(STATEARGS, __getOnEventHandler(STATEARGS, evType), evType);
} /* __triggerOnEventByType */


void __triggerOnEvent(STATEPARAMS, POnEventHandler pHandler,
                      OnEventTypeEnum evType)
/*
 * !!! comment.
 *    params : pHandler == OnEvent handler to call.
 *   returns : probably won't return directly.
 */
{

#warning __triggerOnEvent() BADLY needs commenting!

    unsigned long size;
    void *rc;
    POnEventHandlerPtrs ptrs;
    POnEventsState pState = __getOnEventsState(STATEARGS);

    if (pHandler->handlerAddr == NULL)
        return;

    pState->ptrCount++;
    pState->ptrs = realloc(pState->ptrs, 
                             pState->ptrCount * sizeof (OnEventHandlerPtrs));
    if (pState->ptrs == NULL)
        __fatalRuntimeError(STATEARGS, ERR_INTERNAL_ERROR);
    else
    {
        ptrs = &pState->ptrs[pState->ptrCount - 1];
        ptrs->evType = evType;
        __getBasePointer(&ptrs->basePtr);
        __getStackPointer(&ptrs->stackPtr);

#warning check stack size in __triggerOnEvent()!
        ptrs->protStackSize = size = 
                    ((unsigned long) pHandler->stackPtr) - 
                    ((unsigned long) ptrs->stackPtr);
        ptrs->protectedStack = rc = malloc(size);
        if (rc == NULL)
            __fatalRuntimeError(STATEARGS, ERR_INTERNAL_ERROR);
        else
        {
                /* Copy part of stack that is vulnerable. */
            memcpy(rc, ptrs->stackPtr, size);

                /* Save original return address from stack... */
            ptrs->retAddr = ((void **) ptrs->stackPtr)[1];

                /* Patch stack with new return address... */
            ((void **) ptrs->stackPtr)[1] == &&__onEventRetAddr;
            
                /* Jump into OnEvent handler...yikes. */
            __asm__ __volatile__ ("movl %%ecx, %%esp\n\t"
                                  "movl %%edx, %%ebp\n\t"
                                  "jmpl *(%%eax)\n\t"
                                    : /* no output. */
                                    : "a" (pHandler->handlerAddr),
                                      "c" (pHandler->stackPtr),
                                      "d" (pHandler->basePtr)
                                 );

__onEventRetAddr:
            __asm__ __volatile__ (
                                  "pushl %%eax\n\t"  /* save registers. */
                                  "pushl %%ebx\n\t"
                                  "pushl %%ecx\n\t"
                                  "pushl %%edx\n\t"
                                  "pushl %%esi\n\t"
                                  "pushl %%edi\n\t"
                                  "pushl %%ebx\n\t"
                                  
                                  PUSHNULLSTATEARGS
                                  "call " CLEANUPPROC "\n\t"
                                  POPNULLSTATEARGS
                                  "movl  %%eax, %%ecx\n\t"

                                  "popl %%ebx\n\t"   /* restore registers. */
                                  "popl %%edi\n\t"
                                  "popl %%esi\n\t"
                                  "popl %%edx\n\t"
                                  "popl %%ecx\n\t"
                                  "popl %%ebx\n\t"
                                  "popl %%eax\n\t"
                                  "jmpl *(%%ecx)\n\t"
                                 );
        } /* else */
    } /* else */
} /* __triggerOnEvent */


void *__cleanupOnEventHandler(STATEPARAMS)
/*
 * To avoid using any more inline assembly than needed, this
 *  C routine is called from __triggerOnEvent()'s ASM code to
 *  cleanup after an OnEvent handler returns.
 *
 *      params : void.
 *     returns : address that code would have returned to, had we
 *                not patched it in __triggerOnEvents(). 32-bit return
 *                values are stored in register %eax.
 */
{

#warning check stack location in __cleanupOnEventHandler()!
    
    POnEventsState pState = __getOnEventsState(STATEARGS);
    void *ebp;

    __getBasePointer(&ebp);
    __memReleaseBoxcarsBelow(STATEARGS, (unsigned long) ebp);
    pState->ptrCount--;
    free(pState->ptrs[pState->ptrCount].protectedStack);
    return(pState->ptrs[pState->ptrCount].retAddr);
} /* __cleanupOnEventHandler */


void __doResume(STATEPARAMS, void *resumeAddr)
/*
 *  !!! comment.
 *
 *
 */
{
#warning RESUME functions BADLY need commenting!

    POnEventsState pState = __getOnEventsState(STATEARGS);
    POnEventHandlerPtrs ptrs = &pState->ptrs[pState->ptrCount - 1];
    POnEventHandler pHandler = pState->handlers[pState->handlerCount - 1];

    if (__getInitFlags(STATEARGS) & INITFLAG_DISABLE_RESUME)
        __fatalRuntimeError(STATEARGS, ERR_FEATURE_UNAVAILABLE);

    else
    {
        pState->ptrCount--;
        __asm__ __volatile__ ("std\n\t" /* !!! */
                              "stackrecopy:\n\t"
                              "  lodsb\n\t"
                              "  stosb\n\t"
                              "loop stackrecopy\n\t"
                              "movl %%eax, %%ebp\n\t"
                              "movl %%ebx, %%esp\n\t"
                              "jmpl *(%%edx)\n\t"
                                : /* no output */
                                : "S" (ptrs->protectedStack),
                                  "D" (pHandler->stackPtr),
                                  "a" (ptrs->basePtr),
                                  "b" (ptrs->stackPtr),
                                  "c" (ptrs->protStackSize),
                                  "d" (resumeAddr)
                             );
    } /* else */
} /* __doResume */


void __resumeNext(STATEPARAMS)
{
    POnEventsState pState = __getOnEventsState(STATEARGS);
    POnEventHandlerPtrs ptrs = pState->ptrs[pState->ptrCount - 1];
    POnEventHandler pHandler = pState->handlers[pState->handlerCount - 1];
    void *resumeAddr;
    unsigned long offset;

    resumeAddr = (void *) (((unsigned long) ptrs->basePtr) + 4);
    offset = (unsigned long) pHandler->basePtr - (unsigned long) resumeAddr;
    resumeAddr = (void *) (((unsigned long) ptrs->protectedStack) + offset);
    __doResume(STATEARGS, resumeAddr);
} /* __resumeNext */


void __resumeZero(STATEPARAMS)
{
    POnEventsState pState = __getOnEventsState(STATEARGS);
    POnEventHandlerPtrs ptrs = pState->ptrs[pState->ptrCount - 1];
    POnEventHandler pHandler = pState->handlers[pState->handlerCount - 1];
    void *resumeAddr;
    unsigned long offset;
    
    offset = (unsigned long) pHandler->basePtr - (unsigned long) ptrs->basePtr;
    resumeAddr = (void *) (((unsigned long) ptrs->protectedStack) + offset);
    __doResume(STATEARGS, resumeAddr);
} /* __resumeNext */

/* end of OnEvents.c ... */

