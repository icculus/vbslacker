/*
 * ON EVENT GOTO support routines.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifdef DEBUG
#include <stdio.h>
#endif

#include <stdlib.h>
#include "OnEvents.h"
#include "Initialize.h"
#include "ErrorFunctions.h"
#include "InternalMemManager.h"
#include "Threads.h"
#include "Boolean.h"


#ifdef WIN32
    #define CLEANUPPROC "___cleanupOnEventHandler"
    #define FREEPROC   "_free"
#else
    #define CLEANUPPROC "__cleanupOnEventHandler"
    #define FREEPROC    "free"
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


#ifdef DEBUG

static inline boolean binaryDump(char *fileName, void *data, int size)
{
    FILE *binStream = fopen(fileName, "wb");
    boolean retVal = false;

    if (binStream == NULL)
        printf("  - COULDN'T OPEN [%s] FOR BINARY OUTPUT!\n", fileName);
    else
    {
        if (fwrite(data, size, 1, binStream) != 1)
            printf("  - COULDN'T WRITE DATA DUMP TO DISK!\n");
        else
        {
            printf("  - Data dump is in [%s]...\n", fileName);
            retVal = true;
        }
        fclose(binStream);
    }

    return(retVal);
}

#endif


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


unsigned long __getOnEventsRecursionCount(STATEPARAMS)
{
    return(__getOnEventsState(STATEARGS)->ptrCount);
} /* __getOnEventsRecursionCount */


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

    __deregisterAllOnEventHandlers(STATEARGS);

    __obtainThreadLock(STATEARGS, &onEventsLock);
    pState = ppState[tidx];    
    ppState[tidx] = NULL;
    __releaseThreadLock(STATEARGS, &onEventsLock);

    for (i = 0; i < pState->ptrCount; i++)
        free(pState->ptrs[i].protectedStack);

    free(pState->ptrs);
    free(pState);
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
                              OnEventTypeEnum evType)
/*
 * This is somewhere between "low level but general" and "80x386 specific."
 *
 * Ideally, any module that contains a call to this function should
 *  do something like this:
 *
 *   __setResumeStack;
 *   __registerOnEventHandler(STATEARGS, &&handlerLabel, ONERROR);
 *
 * __setResumeStack is a macro that simply fills the current base and stack
 *  pointers into STATEPARAMS. We need this to be inlined, and done before 
 *  the call to this function begins, so we have a stable stack pointer. The
 *  parser/compiler inserts this macro at the beginning of each function, so
 *  the state should be cool for this call at any time.
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
    POnEventsState pState = __getOnEventsState(STATEARGS);
    int i;
    boolean getOut = false;

        /* !!! make this __fatalRuntimeError()? */
    if ((__stBP == NULL) || (__stSP == NULL))
        __runtimeError(STATEARGS, ERR_INTERNAL_ERROR);

    /* !!! delta skymiles: 2268730674 */
    /* !!! mom's : 215-881-5341 */

        /* determine if we should replace a currently registered handler. */
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
    pHandler->stackPtr = __stSP;
    pHandler->basePtr = __stBP;
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

    for (i = pState->handlerCount - 1;
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

        ptrs->protStackSize = size = 
                    (((unsigned long) pHandler->basePtr) - 
                    ((unsigned long) ptrs->stackPtr)) + 1;
        ptrs->protectedStack = rc = malloc(size);
        if (rc == NULL)
            __fatalRuntimeError(STATEARGS, ERR_INTERNAL_ERROR);
        else
        {
                /* Copy part of stack that is vulnerable. */
            memcpy(rc, ptrs->stackPtr, size);

                /* 
                 * Patch stack to return into here...save original
                 *  return address to ptrs->retAddr for later use.
                 */
            __asm__ __volatile__ ("movl $__onEventRetAddr, %0\n\t" : "=q" (rc));
            ptrs->retAddr = ((void **) pHandler->basePtr)[1];
            ((void **) pHandler->basePtr)[1] = rc;

                /* Jump into OnEvent handler...yikes. */
            __asm__ __volatile__ ("movl 4(%%eax), %%ebp\n\t"     /* spoof ebp */
                                  "movl 8(%%eax), %%esp\n\t"     /* spoof esp */
                                  "jmpl *(%%eax)\n\t"            /* spoof eip */

                   /* --- CODE LANDS HERE IF ONEVENT HANDLER RETURNS. --- */
                                  "__onEventRetAddr:\n\t"

                                  "pushl %%eax\n\t"        /* save registers. */
                                  "pushl %%ebx\n\t"
                                  "pushl %%edx\n\t"
                                  "pushl %%esi\n\t"
                                  "pushl %%edi\n\t"
                                  "pushl %%ebp\n\t"
                                  
                                  PUSHNULLSTATEARGS
                                  "call " CLEANUPPROC "\n\t"
                                  "addl $" STATEARGSSIZESTR ", %%esp\n\t"
                                  "movl  %%eax, %%ecx\n\t"

                                  "popl %%ebp\n\t"      /* restore registers. */
                                  "popl %%edi\n\t"
                                  "popl %%esi\n\t"
                                  "popl %%edx\n\t"
                                  "popl %%ebx\n\t"
                                  "popl %%eax\n\t"
                                                  
                                  "jmpl *%%ecx\n\t" /* return to normal code. */

                                    : /* no output */
                                    : "a" (pHandler)
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
    POnEventsState pState = __getOnEventsState(STATEARGS);
    void *ebp;

    __getBasePointer(&ebp);
    __memReleaseBoxcarsBelow(STATEARGS, (unsigned long) ebp);
    pState->ptrCount--;
    free(pState->ptrs[pState->ptrCount].protectedStack);
    return(pState->ptrs[pState->ptrCount].retAddr);
} /* __cleanupOnEventHandler */


void __doResume(STATEPARAMS, unsigned int retAddrIndex)
#warning RESUME functions BADLY need commenting!
/*
 *  !!! comment.
 *
 *
 */
{
    POnEventsState pState = __getOnEventsState(STATEARGS);
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


void __resumeNext(STATEPARAMS)
{
    __doResume(STATEARGS, 1);
} /* __resumeNext */


void __resumeZero(STATEPARAMS)
{
    __doResume(STATEARGS, 0);
} /* __resumeNext */

/* end of OnEvents.c ... */

