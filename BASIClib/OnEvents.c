/*
 * ON EVENT GOTO support routines.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "OnEvents.h"
#include "InternalMemManager.h"

    /* internal structure for keeping track of registered event handlers... */
typedef struct
{
    int count;
    POnEventHandler *handlers;
} HandlerVector;

typedef HandlerVector *PHandlerVector;


    /* !!! these will need an extra dimension if we start multithreading... */
static HandlerVector table[(OnEventTypeEnum) TOTAL];


void __initOnEvents(void)
/*
 * Call this once before making ANY 'on event' type calls...
 *
 *    params : void.
 *   returns : void.
 */
{
    int i;

    for (i = 0; i < (OnEventTypeEnum) TOTAL; i++)
    {
        table[i].count = 0;
        table[i].handlers = NULL;
    } /* for */
} /* __initOnEvents */


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
    HandlerVector evVect = table[evType];

    if (evVect.count > 0)
        retVal = evVect.handlers[evVect.count - 1];

    return(retVal);
} /* __getOnEventHandler */


void __registerOnEventHandler(void *handlerAddr, void *stackStart,
                              void *stackEnd, void *origReturnAddr,
                              OnEventTypeEnum evType)
/*
 * This is somewhere between "low level but general" and "80x386 specific."
 *
 * Ideally, any module that contains a call to this function should
 *  do something like this:
 *
 *   _stack_ptr_ = getStackPointer();
 *   __registerOnEventHandler(handlerLabel, &lastArg + sizeof(lastArg),
 *                            _stack_ptr_, &firstArg - sizeof(void *),
 *                            ONERROR);
 *
 *
 * _stack_ptr_ should be a (void *) declared at the module level, if needed.
 *  we could theoretically declare one global _stack_ptr_, if threading won't
 *  FUBAR that later. getStackPointer() is an inline assembly macro that simply
 *  gives up the current stack pointer. We need this to be inlined, and done
 *  before the call to this function begins, so we have a stable stack pointer.
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
    HandlerVector evVect = table[evType];

    if ((evVect.count <= 0) ||    /* setup new handler? */
        (evVect.handlers[evVect.count - 1]->stackStart != stackStart))
    {
        /*
         * we can't change evVect.count until the handler is all
         *  set up, since allocating memory MAY throw an exception we'll
         *  want to handle...
         */
        evVect.handlers = __memRealloc(evVect.handlers,
                                      (evVect.count + 1) *
                                         sizeof (POnEventHandler));
        pHandler = __memAlloc(sizeof (OnEventHandler));
        evVect.handlers[evVect.count] = pHandler;
        evVect.count++;
    } /* if */

    else    /* replace current handler... */
        pHandler = evVect.handlers[evVect.count - 1];

    pHandler->handlerAddr = handlerAddr;
    pHandler->stackStart = stackStart;
    pHandler->stackEnd = stackEnd;
    pHandler->origReturnAddr = origReturnAddr;
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
    HandlerVector evVect = table[evType];

    if (evVect.count > 0)
    {
        if (evVect.handlers[evVect.count - 1]->stackStart == stackStart)
        {
            evVect.count--;
            __memFree(evVect.handlers[evVect.count]);
            evVect.handlers = __memRealloc(evVect.handlers,
                                           evVect.count *
                                             (sizeof (POnEventHandler)));
        } /* if */
    } /* if */
} /* __deregisterOnEventHandler */


/* end of OnEvents.c ... */


