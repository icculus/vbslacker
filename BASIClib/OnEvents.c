/*
 * ON EVENT GOTO support routines.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "OnEvents.h"
#include "ErrorFunctions.h"
#include "InternalMemManager.h"
#include "Threads.h"


    /*
     * !!! Needs protection against out of memory errors in allocations...
     * !!! Make sure EVERYTHING in this module won't make recursive calls
     * !!!  to error handlers if we run out of memory while managing error
     * !!!  handlers.
     */


    /* internal structure for keeping track of registered event handlers... */
typedef struct
{
    int count;
    POnEventHandler *handlers;
} HandlerVector;

typedef HandlerVector *PHandlerVector;


    /* Assembly procedure prototypes. */
void __callOnEventHandler(POnEventHandler pHandler);
void __resumeNextHandler(void);

    /*
     * These should be thread safe, since they are only used within
     * The bounds of critical sections.
     */
void *_stack_ptr_ = NULL;
void *_base_ptr_ = NULL;

    /*
     * This is used in __initThreadOnEvents(), to determine whether we need
     *  more memory allocated to handle new threads.
     */
static int threadCount = 0;

    /*
     * This is the array of structures that contain info about all
     *  registered OnEvent handlers. Each structure handles one type
     *  of event, so the array has ((OnEventTypeEnum) TOTAL) elements, and
     *  should be accessed by one of the OnEventTypeEnum members.
     *
     * This has an extra dimension for thread-proofing. SOOO, we have
     *  pTables[__getCurrentThreadIndex()][(OnEventTypeEnum) ONERROR] \ 
     *              .handlers[n]; Ugh.
     */
static PHandlerVector *pTables = NULL;


    /*
     * When we execute the error handler, we lose the contents of every
     *  register. Most importantly, we lose the base pointer (EBP), which
     *  tells us where our parameters and local data can be found on the stack.
     *
     * The only way to keep a reference to anything in that case is to have it
     *  in a global variable, since we always know the location of it. So we
     *  keep a (void **) global var, called basePtrStacks. This variable
     *  is an array of base pointers, since event handling can stack in a
     *  recursive manner. The C code, before calling the event handler,
     *  makes sure there's enough room in the array to handle another base
     *  pointer, and then calls the assembly code. Before executing the
     *  error handler, the assembly procedure will save it's base pointer at
     *  the index specified by basePtrIndexes. After the handler, we have to
     *  start from scratch, so we retrieve our base pointer from where we saved
     *  it, feeling for our digital asshole with both hands and these global
     *  variables. Once returned to a sane state, the ASM code decrements
     *  basePtrIndexes, so that the space used by the stack of base pointers
     *  can begin to shrink.
     *
     * ...of course, this is complicated by thread-protecting our data. :) 
     *  basePtrStacks is actually a (void ***), so we have something roughly
     *  equivalent to basePtrStacks[__getCurrentThreadIndex()] \
     *                             [basePtrIndexes[__getCurrentThreadIndex()]];
     *
     * Ugh.
     */
void ***basePtrStacks = NULL;
int *basePtrIndexes = NULL;

    /*
     * Thread proofed variable (each element holds data for one thread, by
     *  index) containing last triggered On Event type for given thread.
     */
POnEventTypeEnum lastTriggeredOnEventType = NULL;


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
    unsigned int i;

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
        lastTriggeredOnEventType = __memRealloc(lastTriggeredOnEventType,
                                     threadCount * sizeof (POnEventTypeEnum));
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
    basePtrIndexes[tidx] = -1;  /* index set to zero on first call... */
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
    unsigned int i;

    for (i = 0; i < (OnEventTypeEnum) TOTAL; i++)
        __memFree(table[i].handlers);

    __memFree(table);
    __memFree(basePtrStacks[tidx]);
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

    __enterCriticalThreadSection();
    evVect = &pTables[tidx][evType];
    __exitCriticalThreadSection();

    if (evVect->count > 0)
        retVal = evVect->handlers[evVect->count - 1];

    return(retVal);
} /* __getOnEventHandler */


void __registerOnEventHandler(void *handlerAddr, void *stackStart,
                              void *stackEnd, void *basePtr,
                              OnEventTypeEnum evType)
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
 *                            _stack_ptr_, _base_ptr_, ONERROR);
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
 *  actually the "top" of the stack.
 *
 * stackEnd is the stack pointer we saved. The area of memory between
 *  stackStart and stackEnd covers not only arguments and local variables,
 *  but also base ptrs, return addresses, and registers saved on the stack.
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

    __enterCriticalThreadSection();
    evVect = &pTables[tidx][evType];
    __exitCriticalThreadSection();

    if ((evVect->count <= 0) ||    /* setup new handler? */
        (evVect->handlers[evVect->count - 1]->stackStart != stackStart))
    {
        /*
         * we can't change evVect->count until after the _mem*lloc() calls,
         *  since allocating memory MAY throw an exception we'll want to
         *  handle...
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
    pHandler->basePtr = basePtr;
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

    __enterCriticalThreadSection();
    theStack = basePtrStacks[tidx];
    bpIndex = basePtrIndexes[tidx];
    retVal = &theStack[bpIndex];
    __exitCriticalThreadSection();

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

    __enterCriticalThreadSection();
    basePtrIndexes[tidx]++;
    basePtrStacks[tidx] = __memRealloc(basePtrStacks[tidx],
                                      (basePtrIndexes[tidx] + 1) *
                                         sizeof (void *));
    lastTriggeredOnEventType[tidx] = evType;
    __exitCriticalThreadSection();

    __callOnEventHandler(pHandler);     /* initialize miracle mode... */
} /* __triggerOnEvent */


void __resumeNext(void)
{
    POnEventHandler pHandler;
    int tidx = __getCurrentThreadIndex();
    OnEventTypeEnum evType;

    __enterCriticalThreadSection();
    evType = lastTriggeredOnEventType[tidx];
    __exitCriticalThreadSection();

    pHandler = __getOnEventHandler(evType);

    if (pHandler == NULL)
        __runtimeError(ERR_NO_RESUME);    /* !!! or ERR_RESUME_WITHOUT_ERROR? */
    else
        __resumeNextHandler();
} /* __resumeNext */

/* end of OnEvents.c ... */

