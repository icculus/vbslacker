/*
 * Routines for internal BASIClib runtime error handling.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicError.h"


/*
 * We keep an extra copy of the BASIC error number around. Both are set
 *  equally in case of an error, but otherwise, "__basicErrno" is
 *  ignored. This allows internal BASIClib routines to reset the errno to
 *  NO_ERROR, call something, and test for error, all without altering
 *  the errno that is accessable by outside programs.
 */
int basicErrno = ERR_NO_ERROR;
int __basicErrno = ERR_NO_ERROR;

static char *errStrings[MAX_ERRS];

void __initErrorFunctions(STATEPARAMS)
/*
 * Initialize the table of errorStrings.
 *
 *  !!! write this later. Internationalization should be considered, eh?
 */
{
    memset(errStrings, '\0', sizeof (errStrings));
} /* __initErrorFunctions */


void __defaultRuntimeErrorHandler(STATEPARAMS)
{
    char *errStr;

    errStr = ((basicErrno > MAX_ERRS) ? UNKNOWN_ERR : errStrings[basicErrno]);
    if (errStr == NULL)
        errStr = UNKNOWN_ERR;

    printf("\n\n***Unhandled runtime error***\n"
           "  \"%s\" (#%d)\n"
           "    - __stIP     == (%p)\n"
           "    - __stNextIP == (%p)\n"
           "    - __stBP     == (%p)\n"
           "    - __stSP     == (%p)\n"
           "\n\n",
            errStr, basicErrno, __stIP, __stNextIP, __stBP, __stSP);

    exit(basicErrno);
} /* __defaultRuntimeErrorHandler */


void __fatalRuntimeError(STATEPARAMS, int errorNum)
/*
 * Call this instead of __runtimeError() if you want to throw an
 *  unrecoverable error. Even ERR_NO_ERROR is fatal here.
 *
 *    params : errorNum == error number to throw.
 *   returns : never.
 */
{
    basicErrno = __basicErrno = errorNum;
    __defaultRuntimeErrorHandler(STATEARGS);
} /* __fatalRuntimeError */



void __runtimeError(STATEPARAMS, int errorNum)
{
    POnEventHandler pHandler;

    basicErrno = __basicErrno = errorNum;

    if (errorNum != ERR_NO_ERROR)
    {
        pHandler = __getOnEventHandler(STATEARGS, ONERROR);
        if ((pHandler == NULL) || (pHandler->handlerAddr == NULL))
            __defaultRuntimeErrorHandler(STATEARGS);
        else
            __triggerOnEvent(STATEARGS, pHandler, ONERROR);
    } /* if */
} /* __runtimeError */


/* end of BasicError.c ... */

