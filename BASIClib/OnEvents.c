/*
 * ON EVENT GOTO support routines.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "OnEvents.h"
#include "InternalMemManager.h"
#include "Threads.h"


    /*
     * !!! Needs protection against out of memory errors in allocations...
     * !!! Make sure EVERYTHING in this module won't make recursive calls
     * !!!  to error handlers if we run out of memory while managing error
     * !!!  handlers.
     */


/* !!! We need to document the allocation strategy and data structuring
   !!!   in here. It's gotten complicated with thread-proofing... */



    /* internal structure for keeping track of registered event handlers... */
typedef struct
{
    int count;
    POnEventHandler *handlers;
} HandlerVector;

typedef HandlerVector *PHandlerVector;


    /* Assembly procedure prototype. */
void __callOnEventHandler(POnEventHandler pHandler);


    /*
     * These should be thread safe, since they are only used within
     * The bounds of critical sections.
     */
void *_stack_ptr_ = NULL;
void *_base_ptr_ = NULL;

static int threadCount = 0;

    /* These have an extra dimension for thread-proofing. */
static PHandlerVector *pTables = NULL;
void ***basePtrStacks = NULL;
int *basePtrIndexes = NULL;



#ifdef DEBUG

/* !!! lose this. */ 

#include <stdio.h>

int spacer = 0;
void printTabs(int spaces)
{
    int i;
    int max = spaces * 4;

    for (i = 0; i < max; i++)
        printf(" ");
}    

void printText(int spaces, char *text)
{
    printTabs(spaces);
    printf("%s", text);
}

#endif


void __initThreadOnEvents(int tidx)
/*
 * This makes sure space exists in the thread-protected arrays for the
 *  current thread index. This is called whenever a new thread is created.
 *
 *      params : void.
 *     returns : void, but all the tables could get __memRealloc()ed.
 */
{
    PHandlerVector table;
    int currentThreadCount;
    int i;

    printText(spacer++, "__initThreadsOnEvents(");
    printf("%d)...\n", tidx);

    __enterCriticalThreadSection();

    currentThreadCount = __getHighestThreadIndex() + 1;
    if (threadCount != currentThreadCount)
    {        
        threadCount = currentThreadCount;

        pTables = __memRealloc(pTables, threadCount * sizeof(PHandlerVector *));
        basePtrStacks = __memRealloc(basePtrStacks,
                                     threadCount * sizeof (void *));
        basePtrIndexes = __memRealloc(basePtrIndexes,
                                     threadCount * sizeof (int));
    } /* if */

    __exitCriticalThreadSection();

    table = __memAlloc(sizeof (HandlerVector) * (OnEventTypeEnum) TOTAL);
    pTables[tidx] = table;
    for (i = 0; i < (OnEventTypeEnum) TOTAL; i++)
    {
        table[i].count = 0;
        table[i].handlers = NULL;
    } /* for */

    basePtrStacks[tidx] = NULL;
    basePtrIndexes[tidx] = -1;
    printText(--spacer, "__initThreadsOnEvent()...\n");
} /* __initThreadOnEvents */


void __deinitThreadOnEvents(int tidx)
/*
 * A thread has died; free its resources.
 *
 *    params : tidx == dead thread's index.
 *   returns : void.
 */
{
    PHandlerVector table;    
    int i;

    printText(spacer++, "__deinitThreadsOnEvents(");
    printf("%d)...\n", tidx);

    for (i = 0; i < (OnEventTypeEnum) TOTAL; i++)
        __memFree(table[i].handlers);

    __memFree(table);
    __memFree(basePtrStacks[tidx]);

    printText(--spacer, "__deinitThreadsOnEvent()...\n");
} /* __deinitThreadOnEvents */


POnEventHandler __getOnEventHandler(OnEventTypeEnum evType)
/*
 * Returns the current ON EVENT GOTO handler for event type evType.
 *
 *    params : evType == type of event we want handler for.
 *   returns : most significant handler for given event type. NULL if
 *              there isn't one.
 */
{
    POnEventHandler retVal = NULL;
    PHandlerVector evVect;
    int tidx = __getCurrentThreadIndex();

    printText(spacer++, "__getOnEventHandler(");
    printf("%d)...\n", (int) evType);


    __enterCriticalThreadSection();
    evVect = &pTables[tidx][evType];
    __exitCriticalThreadSection();

    if (evVect->count > 0)
        retVal = evVect->handlers[evVect->count - 1];

    printText(--spacer, "__getOnEventHandler()...\n");


    return(retVal);
} /* __getOnEventHandler */


void __registerOnEventHandler(void *handlerAddr, void *stackStart,
                              void *stackEnd, void *origReturnAddr,
                              void *basePtr, OnEventTypeEnum evType)
/*
 * This is somewhere between "low level but general" and "80x386 specific."
 *
 * Ideally, any module that contains a call to this function should
 *  do something like this:
 *
 *   __enterCriticalThreadSection();
 *   __getStackPointer(&_stack_ptr_);
 *   __getBasePointer(&_base_ptr);
 *   __registerOnEventHandler(&&handlerLabel, &lastArg + sizeof(lastArg),
 *                            _stack_ptr_, &firstArg - sizeof(void *),
 *                            _base_ptr_, ONERROR);
 *   __exitCriticalThreadSection();
 *
 *
 * _stack_ptr_ should be a (void *) declared at the module level, if needed.
 *  we could theoretically declare one global _stack_ptr_, if threading won't
 *  FUBAR that later. getStackPointer() is an inline assembly macro that simply
 *  gives up the current stack pointer. We need this to be inlined, and done
 *  before the call to this function begins, so we have a stable stack pointer.
 *
 * Same thing applies to _base_ptr_.
 *
 * handlerAddr is the goto label we'll be blindly jumping to to handle the
 *  the runtime error.
 *
 * stackStart is the address of the LAST argument of the calling function, plus
 *  the size of it.  Since C pushes arguments on the stack backwards, this is
 *  actually the "top" of the stack. !!! minus one?
 *
 * stackEnd is the stack pointer we saved. The area of memory between
 *  stackStart and stackEnd covers not only arguments and local variables,
 *  but also base ptrs, return addresses, and registers saved on the stack.
 *
 * origReturnAddr is the location on the stack of the pointer to where the
 *  function in question was originally called from. We can only find this
 *  while we specifically know what the arguments are; the address is on
 *  the stack directly before the first argument to the function, as it is
 *  pushed last, after all the arguments. So we take the address of the first
 *  arg and subtract the size of a void ptr, which should give us access to
 *  the caller's address. We need to patch that address with a new one when
 *  we are ready to call the event handler.
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
 *  previous one, based on the stackStart argument, which will be constant
 *  for all calls from a single procedure.
 *
 *     returns : void.
 */
{
    POnEventHandler pHandler = NULL;
    PHandlerVector evVect;
    int tidx = __getCurrentThreadIndex();

    printText(spacer++, "__registerOnEventHandler()...\n");


    __enterCriticalThreadSection();
    evVect = &pTables[tidx][evType];
    __exitCriticalThreadSection();

    if ((evVect->count <= 0) ||    /* setup new handler? */
        (evVect->handlers[evVect->count - 1]->stackStart != stackStart))
    {
        /*
         * we can't change evVect->count until the handler is all
         *  set up, since allocating memory MAY throw an exception we'll
         *  want to handle...
         */
        evVect->handlers = __memRealloc(evVect->handlers,
                                       (evVect->count + 1) *
                                         sizeof (POnEventHandler));
        pHandler = __memAlloc(sizeof (OnEventHandler));
        evVect->handlers[evVect->count] = pHandler;
        evVect->count++;
    } /* if */

    else    /* replace current handler... */
        pHandler = evVect->handlers[evVect->count - 1];

    pHandler->handlerAddr = handlerAddr;
    pHandler->stackStart = stackStart;
    pHandler->stackEnd = stackEnd;
    pHandler->origReturnAddr = origReturnAddr;
    pHandler->basePtr = basePtr;
    printText(--spacer, "__registerOnEventHandler()...\n");
} /* __registerOnEventHandler */


void __deregisterOnEventHandler(void *stackStart, OnEventTypeEnum evType)
/*
 * Call this to deregister an event handler. Calls to this will be automatically
 *  generated by the parser/compiler when needed, for commands such as
 *  ON ERROR GOTO 0 and EXIT SUB (calls are added for EXIT SUB/FUNCTION commands
 *  in any procedures that enable some sort of ON EVENT GOTO ...) Calling
 *  this function more than once is harmless, so if ON ERROR GOTO 0 is used
 *  right before an EXIT SUB, this function will ignore the second call.
 *
 *      params : *stackStart == pointer passed as stackStart parameter in
 *                              __registerOnEventHandler().
 *               evType      == Event type, for organizational purposes.
 *     returns : void.
 */
{
    int tidx = __getCurrentThreadIndex();    
    PHandlerVector evVect;

    printText(spacer++, "__deregisterOnEventHandler()...\n");

        /* !!! can we separate this into another function? */
    __enterCriticalThreadSection();
    evVect = &pTables[tidx][evType];
    __exitCriticalThreadSection();

    if (evVect->count > 0)
    {
        if (evVect->handlers[evVect->count - 1]->stackStart == stackStart)
        {
            evVect->count--;
            __memFree(evVect->handlers[evVect->count]);
            evVect->handlers = __memRealloc(evVect->handlers,
                                            evVect->count *
                                             (sizeof (POnEventHandler)));
        } /* if */
    } /* if */
    printText(--spacer, "__deregisterOnEventHandler()...\n");
} /* __deregisterOnEventHandler */


void **__calcBasePtrStorage(void)
/*
 * Figure out precisely where the current storage location for
 *  this thread's base pointer stack is.
 *
 *    params : void.
 *   returns : pointer to (void *) storage location.
 */
{
    int tidx = __getCurrentThreadIndex();
    int bpIndex;
    void **theStack;
    void **retVal;

    printText(spacer++, "__calcBasePtrStorage()...\n");

    __enterCriticalThreadSection();
    theStack = basePtrStacks[tidx];
    bpIndex = basePtrIndexes[tidx];
    retVal = &theStack[bpIndex];
    __exitCriticalThreadSection();

    printText(spacer, "tidx == (");
    printf("%d)    index == (%d)\n", tidx, basePtrIndexes[tidx]);

    printText(spacer, "retVal = (");
    printf("%p)\n", retVal);

    printText(--spacer, "__calcBasePtrStorage()...\n");
    return(retVal);
} /* __calcBasePtrStorage */


void __triggerOnEvent(OnEventTypeEnum evType)
/*
 * This functions sets up a globally accessable buffer for the ASM routine
 *  (which it can access when the stack state is FUBAR), and calls the
 *  routine.
 *
 *    params : evType == Event type to handler.
 *   returns : should never return directly, due to stack voodoo.
 */
{
     /* !!! Needs protection against out of memory errors in allocations... */
    POnEventHandler pHandler = __getOnEventHandler(evType);
    int tidx = __getCurrentThreadIndex();

    printText(spacer++, "__triggerOnEvent()...\n");

    __enterCriticalThreadSection();
    basePtrIndexes[tidx]++;
    basePtrStacks[tidx] = __memRealloc(basePtrStacks[tidx],
                                      (basePtrIndexes[tidx] + 1) *
                                         sizeof (void *));
    __exitCriticalThreadSection();

    __callOnEventHandler(pHandler);
    printText(--spacer, "__triggerOnEvent()...\n");
} /* __triggerOnEvent */


/* end of OnEvents.c ... */

