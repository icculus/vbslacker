/*
 * ON EVENT GOTO support routines.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdlib.h>
#include <malloc.h>
#include "OnEvents.h"


    /*
     * There is one OnEventsHandler object for each registered handler.
     *  These objects store data about each handler's behavior.
     */
typedef struct
{
    __integer evType;             /* Type of event this handler handles.  */
    void *handlerAddr;            /* Code address of handler.             */
    void *basePtr;                /* Base pointer in handler's function.  */
    void *stackPtr;               /* Stack pointer in handler's function. */
    __boolean isActive;           /* Is handler active?                   */
} OnEventHandler;

typedef OnEventHandler *POnEventHandler;


    /*
     * There is one OnEventThreadState object for each thread. They
     *  keep track of their respective thread's OnEvent handlers.
     */
typedef struct
{
    POnEventHandler handlers;     /* List of registered handlers.         */
    unsigned long handlerCount;   /* Number of registered handlers.       */
} OnEventThreadState;

typedef OnEventThreadState *POnEventsThreadState;


    /*
     * module-scope ThreadLock, for protecting (ppState) from race conditions.
     */
static ThreadLock onEventsLock;

    /*
     * ppState stores the OnEventThreadState structures used by each thread.
     *  This is an array of pointers, so we only have to use ThreadLocks when
     *  accessing this dynamic array, and not the individual structures.
     */
static POnEventThreadState *ppState = NULL;


/* internal function prototype... */
static void __deregisterAllOnEventHandlers(STATEPARAMS);


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


void __initThreadOnEvents(STATEPARAMS, int tidx)
/*
 * This makes sure space exists in the thread-protected arrays for the
 *  current thread index. This is called whenever a new thread is created.
 *
 *      params : void.
 *     returns : void, but all the tables could get __memRealloc()ed.
 */
{
    POnEventThreadState pState;

    pState = calloc(1, sizeof (OnEventThreadState));
    if (pState == NULL)
        __fatalRuntimeError(STATEARGS, ERR_INTERNAL_ERROR);

    __obtainThreadLock(STATEARGS, &onEventsLock);
    ppState = realloc(ppState, (__getHighestThreadIndex(STATEARGS) + 1) *
                                     sizeof (POnEventThreadState));

    if (ppState == NULL)
        __fatalRuntimeError(STATEARGS, ERR_INTERNAL_ERROR);

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
    POnEventThreadState pState;
    unsigned int i;

    __deregisterAllOnEventHandlers(STATEARGS);

    __obtainThreadLock(STATEARGS, &onEventsLock);
    pState = ppState[tidx];    
    ppState[tidx] = NULL;
    __releaseThreadLock(STATEARGS, &onEventsLock);

    free(pState->handlers);
    free(pState);
} /* __deinitThreadOnEvents */


POnEventThreadState __getOnEventThreadState(STATEPARAMS)
/*
 * Get a pointer to current thread's OnEventThreadState structure. This
 *  takes care of all the thread-proofing details.
 *
 *     params : void.
 *    returns : Pointer to current OnEventThreadState.
 */
{
    POnEventThreadState retVal;

    __obtainThreadLock(STATEARGS, &onEventsLock);
    retVal = ppState[__getCurrentThreadIndex(STATEARGS)];
    __releaseThreadLock(STATEARGS, &onEventsLock);

    return(retVal);
} /* __getOnEventThreadState */


POnEventHandler __getOnEventHandler(STATEPARAMS, __integer evType)
/*
 * Returns the current ON EVENT GOTO handler for event type evType.
 *  Ignores handlers that are currently active.
 *
 * If we hit an event handler ADDRESS for the specified type that is NULL,
 *  then we stop looking, since this signifies an ON EVENT GOTO 0 command,
 *  which disables handling for that OnEvent.
 *
 *    params : evType == type of event we want handler for.
 *   returns : Most significant handler for given event type. NULL if there
 *              isn't one, or handling has been disabled for this event.
 */
{
    POnEventHandler retVal = NULL;
    POnEventHandler *vect;
    POnEventThreadState pState = __getOnEventThreadState(STATEARGS);
    unsigned int i;

    for (i = pState->handlerCount - 1, vect = pState->handlers;
        (i >= 0) && (retVal == NULL); i--)
    {
        if ((vect[i]->evType == evType) && (vect[i]->isActive == false))
            retVal = vect[i];
    } /* for */

        /*
         * "On Event Goto 0" handlers are never "active", so code will fall
         *  here if we stumble across one...but we need to check for it...
         *  these are signified by a NULL in the handlerAddr field.
         */
    return((retVal->handlerAddr == NULL) ? NULL : retVal);
} /* __getOnEventHandler */


void __registerOnEventHandler(STATEPARAMS, void *handlerAddr, void *base,
                              void *stack, int evType)
/*
 * This is somewhere between "low level but general" and "80x386 specific."
 *
 * Ideally, any module that contains a call to this function should
 *  do something like this:
 *
 *   __setStateStack;
 *   __setStateInstructs(&&l0, &&l1);
 *   l0:
 *   __registerOnEventHandler(STATEARGS, &&handlerLabel, ONERROR);
 *   l1:
 *
 * __setStateStack is a macro that simply fills the current base and stack
 *  pointers into STATEPARAMS. We need this to be inlined, and done before 
 *  the call to this function begins, so we have a stable stack pointer. The
 *  parser/compiler inserts this macro at the beginning of each function, so
 *  the state should be cool for this call at any time.
 *
 * __setStateInstructs is a macro that sets the addresses that will be
 *  jumped to for RESUME 0 and RESUME NEXT commands.
 *
 * handlerAddr is the goto label we'll be blindly jumping to to handle the
 *  the runtime error. NULL is like ON ERROR GOTO 0.
 *
 * evType just guarantees that we don't call a timer handler for a runtime
 *  error, etc...
 *
 *    (*whew!*)   !!! rewrite all of this!
 *
 * All that said, call this function to register an event handler with
 *  BASIClib that will be called when appropriate. Ideally, the parser/compiler
 *  will generate all the calls to this function when it finds ON EVENT GOTO
 *  commands.
 *
 * Only one event handler of each type can be active for any given procedure.
 *  This function will check the handler to see if it should replace a
 *  previous one, based on base pointers, which will be constant for all
 *  calls from an instance of a single procedure.
 *
 *     returns : void.
 */
{
    POnEventHandler pHandler = NULL;
    POnEventThreadState pState = __getOnEventThreadState(STATEARGS);
    int i;
    __boolean getOut = false;

        /*
         * Sanity check; make sure the base and stack pointers are valid...
         */
    if ((__stBP == NULL) || (__stSP == NULL))
        __fatalRuntimeError(STATEARGS, ERR_INTERNAL_ERROR);

        /*
         * Determine if we should replace a currently registered handler.
         *  This is determined by whether any handlers exist for the
         *  specified base pointer, and if so, if there are any of the
         *  desired type. If neither condition is true, then we aren't
         *  replacing a previous handler.
         */
    for (i = pState->handlerCount - 1; (i >= 0) && (getOut == false); i--)
    {
        if (pState->handlers[i]->basePtr != __stBP)
            getOut = true;
        else if (pState->handlers[i]->evType == evType)
        {
            getOut = true;
            pHandler = pState->handlers[i];
        } /* else if */
    } /* for */

        /*
         * If there's no previously registered handler,
         *  allocate space for a new one...
         */
    if (pHandler == NULL)
    {
            /* allocate space in array for new handler. */
        pState->handlerCount++;
        pState->handlers = realloc(pState->handlers, pState->handlerCount *
                                                     sizeof (OnEventHandler));

        if (pState->handlers == NULL)
            __fatalRuntimeError(STATEARGS, ERR_INTERNAL_ERROR);

        pHandler = &(pState->handlers[pState->handlerCount - 1]);
    } /* if */

        /*
         * Set up fields in handler structure...
         */
    pHandler->evType = evType;
    pHandler->handlerAddr = handlerAddr;
    pHandler->basePtr = __stBP;
    pHandler->stackPtr = __stSP;
    pHandler->isActive = false;
} /* __registerOnEventHandler */


static void __deregisterAllOnEventHandlers(STATEPARAMS)
/*
 * This function does the same thing as __deregisterOnEventHandlers(), but
 *  considers all OnEvent handlers irrelevant, thus removing them all.
 *  Careful with this one.
 *
 *     params : void.
 *    returns : void.
 */
{
    int i;
    POnEventThreadState pState = __getOnEventThreadState(STATEARGS);

    free(pState->handlers);
    pState->handlers = NULL;
    pState->handlerCount = 0;
} /* __deregisterAllOnEventHandlers */


void __deregisterOnEventHandlers(STATEPARAMS)
/*
 * Call this to deregister irrelevant event handlers. All handlers
 *  past the end of the current stack are assumed to be useless, and
 *  are deregistered.
 *
 *      params : void.
 *     returns : void.
 */
{
#warning rewrite parts of __deregisterOnEventHandlers()!

    POnEventThreadState pState = __getOnEventThreadState(STATEARGS);
    int i;

    for (i = pState->handlerCount - 1;
            (i >= 0) && (pState->handlers[i]->basePtr <= ebp);
            i--)
    {
        pState->handlerCount--;
        free(pState->handlers[i]);
    } /* for */

    ppHandler = realloc(pState->handlers,
                        pState->handlerCount * sizeof (OnEventHandler));

    if (ppHandler != NULL)
        pState->handlers = ppHandler;
} /* __deregisterOnEventHandlers */


__boolean __triggerOnEvent(STATEPARAMS, __integer evType)
/*
 * This function activates the correct OnEvent handler, based on event type.
 *  Will only call a handler if it a) exists, and b) is not currently active.
 *
 *    params : evType == Event type to handle.
 *   returns : either won't return directly, due to stack voodoo, or if no
 *              event handler of specified type is available, returns (false).
 */
{
    POnEventHandler pHandler = __getOnEventHandler(STATEARGS, evType);

    if (pHandler == NULL)       /* No handler available?  Bail out. */
        return(false);

    pHandler->isActive = true;
    __setStackAndJump(pHandler->basePtr,
                      pHandler->stackPtr,
                      pHandler->handlerAddr);
} /* __triggerOnEvent */


void __doResume(STATEPARAMS, unsigned int retAddrIndex)
/*
 * Basic's RESUME command. If RESUME support hasn't been disabled,
 *  and everything goes correctly, this will make the code flow jump
 *  to the address of the event-triggering statement, or the next
 *  statement after the event.
 *
 *     params : retAddrIndex == if 0, RESUME 0, if 1, RESUME NEXT.
 *    returns : Should never return, as code jumps elsewhere.
 */
{
    POnEventThreadState pState = __getOnEventThreadState(STATEARGS);
    POnEventHandlerPtrs ptrs = &pState->ptrs[pState->ptrCount - 1];
    POnEventHandler pHandler = pState->handlers[pState->handlerCount - 1];
    void **offset;
    void *resumeAddr;
    void *bp;
    void *sp;

    offset = (void **) ( (unsigned long) ptrs->protectedStack +
                            ( (unsigned long) ptrs->basePtr -
                              (unsigned long) ptrs->stackPtr ) );

    offset += 2;    /* 1 for stored base pointer, 1 for return address. */

    resumeAddr = offset[retAddrIndex];
    bp = offset[2];
    sp = offset[3];

    if (__getInitFlags(STATEARGS) & INITFLAG_DISABLE_RESUME)
        __fatalRuntimeError(STATEARGS, ERR_NO_RESUME);

    if ((resumeAddr == NULL) || (bp == NULL) || (sp == NULL))
        __fatalRuntimeError(STATEARGS, ERR_INTERNAL_ERROR);

    else
    {
        pState->ptrCount--;

            /* repatch original return address on stack... */ 
        ((void **) pHandler->basePtr)[1] = ptrs->retAddr;

        __asm__ __volatile__ (                 /* Setup all the registers... */
                              "movl 8(%%ecx), %%edi\n\t"   /* ptrs->stackPtr */
                              "movl %%eax, %%ebp\n\t"      /* bp             */
                              "movl %%esi, %%esp\n\t"      /* sp             */
                              "movl 12(%%ecx), %%esi\n\t"  /* protectedStack */
                              "movl %%esi, %%ebx\n\t"      /* save a copy.   */
                              "movl 16(%%ecx), %%ecx\n\t"  /* protStackSize  */

                                   /* recopy the protected stack into place. */
                              "cld\n\t"                    /* copy direction */
                              "stackrecopy:\n\t"
                              "  lodsb\n\t"
                              "  stosb\n\t"
                              "decl %%ecx\n\t"
                              "orl %%ecx, %%ecx\n\t"       /* does ecx == 0? */
                              "jnz stackrecopy\n\t"      /* "loop" is buggy. */

                                            /* free() the protected stack... */
                              "pushl %%edx\n\t"
                              "pushl %%ebx\n\t"
                              "call " FREEPROC "\n\t"
                              "addl $4, %%esp\n\t"
                              "popl  %%edx\n\t"

                                            /* jump to resume code position. */
                              "jmpl *%%edx\n\t"

                                : /* no output */
                                : "c" (ptrs),
                                  "a" (bp),
                                  "S" (sp),
                                  "d" (resumeAddr)
                             );
    } /* else */
} /* __doResume */

/* end of OnEvents.c ... */

