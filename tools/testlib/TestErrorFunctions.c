/*
 * Test error functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include "OnEvents.h"
#include "ErrorFunctions.h"


void test___runtimeError(STATEPARAMS)
/*
 * Test __runtimeError() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("Testing __runtimeError()...\n");

    __setResumeStack;
    __registerOnEventHandler(STATEARGS, &&errError, ONERROR);

    __basicErrno = ERR_NO_ERROR;
    __runtimeError(STATEARGS, ERR_TOO_MANY_FILES);
    printf("  - Didn't call error handler.\n");
    __deregisterOnEventHandlers(STATEARGS);
    return;

errError:              /* error handler... */ 
    if (__basicErrno != ERR_TOO_MANY_FILES)
    {
        printf("  - Threw error (%d), should have thrown (%d)!\n",
                             __basicErrno, ERR_TOO_MANY_FILES);

    } /* if */

    __deregisterOnEventHandlers(STATEARGS);
} /* test___runtimeError */


void test_func_err(STATEPARAMS)
/*
 * Test err() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    int rc;

    printf("Testing err() function...\n");

    __setResumeStack;
    __registerOnEventHandler(STATEARGS, &&errError, ONERROR);

    __basicErrno = ERR_NO_ERROR;
    __runtimeError(STATEARGS, ERR_TOO_MANY_FILES);
    printf("  - Didn't call error handler.\n");
    __deregisterOnEventHandlers(STATEARGS);
    return;

errError:              /* error handler... */ 
    rc = func_err(STATEARGS);

    if (rc != ERR_TOO_MANY_FILES)
    {
        printf("  - returned (%d), should have returned (%d)!\n",
                        rc, ERR_TOO_MANY_FILES);
    } /* if */

    __deregisterOnEventHandlers(STATEARGS);
} /* test_func_err */



void test_proc_err(STATEPARAMS)
/*
 * Test "err = errno" functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    int rc;

    printf("Testing err() procedure...\n");

    proc_err(STATEARGS, ERR_LABEL_NOT_DEFINED);
    rc = func_err(STATEARGS);
    if (rc != ERR_LABEL_NOT_DEFINED)
    {
        printf("  - returned (%d), should have returned (%d)!\n",
                        rc, ERR_LABEL_NOT_DEFINED);
    } /* if */
} /* test_proc_err */



void testErrorFunctions(STATEPARAMS)
/*
 * This code tests all the error functions in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n[TESTING ERROR FUNCTIONS...]\n");

    test___runtimeError(STATEARGS);
    test_func_err(STATEARGS);
    test_proc_err(STATEARGS);
} /* testErrorFunctions */


#ifdef STANDALONE

#include "Initialize.h"

int main(void)
{
    __initBasicLib(NULLSTATEARGS, INITFLAG_NO_FLAG);
    testErrorFunctions(NULLSTATEARGS);
    __deinitBasicLib(NULLSTATEARGS);
} /* main */

#endif

/* end of TestErrorFunctions.c ... */
