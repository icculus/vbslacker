/*
 * ON EVENT GOTO support routines.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */


/* !!! needs commenting BADLY. */



#include "OnEvents.h"
#include "InternalMemManager.h"

typedef struct
{
    int count;
    POnEventHandler *handlers;
} HandlerVector;

typedef HandlerVector *PHandlerVector;


    /* module-scope variables... */
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
                              void *stackEnd, OnEventTypeEnum evType)
{   
    POnEventHandler pHandler = NULL;
    HandlerVector evVect = table[evType];

    /* EnterCritSection()... */

    if ((evVect.count <= 0) ||
        (evVect.handlers[evVect.count - 1]->stackStart != stackStart))
    {
        /*
         * we can't change evVect.count until the handler is all
         *  set up, since allocating memory MAY throw an exception we'll
         *  want to handle.
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

    /* ExitCritSection()... */
} /* __registerOnEventHandler */


void __deregisterOnEventHandler(void *stackStart, OnEventTypeEnum evType)
{
    /* EnterCritSection() */

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

    /* ExitCritSection() */

} /* __deregisterOnEventHandler */


/* end of OnEvents.c ... */


