/*
 * BASIC runtime error subsystem.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "ErrorFunctions.h"
#include "OnEvents.h"

int basicErrno = (RuntimeErrEnum) NO_ERROR;

void __runtimeError(RuntimeErrEnum errorNum)
{
    POnEventHandler pHandler;

    basicErrno = errorNum;

    pHandler = getOnEventHandler(ONERROR);
    if (pHandler == NULL)
        __defaultRuntimeErrorHandler();
    else
        __callOnEventHandler(pHandler);
} /* __runtimeError */


double func_err(void)
{
    return((double) basicErrno);
} /* func_err */


void proc_err(double newErr)
{
    /* !!! check this later. Fuck. Double vs. int vs. enum. */

    basicErrno = newErr;
} /* proc_err */


/* end of ErrorFunctions.c ... */


