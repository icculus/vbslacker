/*
 * Test error functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include "OnEvents.h"
#include "ErrorFunctions.h"


void test___runtimeError(int x)  /* !!! */
/*
 * Test __runtimeError() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("Testing __runtimeError()...\n");

    __obtainThreadLock(&registerLock);
    __getStackPointer(&_stack_ptr_);
    __getBasePointer(&_base_ptr_);
    __registerOnEventHandler(&&errError, &x + sizeof (x),
                             _stack_ptr_, _base_ptr_, ONERROR);
    __releaseThreadLock(&registerLock);


    __basicErrno = ERR_NO_ERROR;
    __runtimeError(ERR_TOO_MANY_FILES);
    printf("  - Didn't call error handler.\n");
    __deregisterOnEventHandler(&x + sizeof (x), ONERROR);
    return;

errError:              /* error handler... */ 
    __markOnEventHandlerAddr;

    if (__basicErrno != ERR_TOO_MANY_FILES)
    {
        printf("  - Threw error (%d), should have thrown (%d)!\n",
                             __basicErrno, ERR_TOO_MANY_FILES);

    } /* if */

    __deregisterOnEventHandler(&x + sizeof (x), ONERROR);
} /* test___runtimeError */


void test_func_err(int x) /* !!! */
/*
 * Test err() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    int rc;

    printf("Testing err() function...\n");

    __obtainThreadLock(&registerLock);
    __getStackPointer(&_stack_ptr_);
    __getBasePointer(&_base_ptr_);
    __registerOnEventHandler(&&errError, &x + sizeof (x),
                             _stack_ptr_, _base_ptr_, ONERROR);
    __releaseThreadLock(&registerLock);


    __basicErrno = ERR_NO_ERROR;
    __runtimeError(ERR_TOO_MANY_FILES);
    printf("  - Didn't call error handler.\n");
    __deregisterOnEventHandler(&x + sizeof (x), ONERROR);
    return;

errError:              /* error handler... */ 
    __markOnEventHandlerAddr;

    rc = func_err();

    if (rc != ERR_TOO_MANY_FILES)
    {
        printf("  - returned (%d), should have returned (%d)!\n",
                        rc, ERR_TOO_MANY_FILES);
    } /* if */

    __deregisterOnEventHandler(&x + sizeof (x), ONERROR);
} /* test_func_err */



void test_proc_err(void)
/*
 * Test "err = errno" functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    int rc;

    printf("Testing err() procedure...\n");

    proc_err(ERR_LABEL_NOT_DEFINED);
    rc = func_err();
    if (rc != ERR_LABEL_NOT_DEFINED)
    {
        printf("  - returned (%d), should have returned (%d)!\n",
                        rc, ERR_LABEL_NOT_DEFINED);
    } /* if */
} /* test_proc_err */



void testErrorFunctions(void)
/*
 * This code tests all the error functions in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n[TESTING ERROR FUNCTIONS...]\n");

    test___runtimeError(1);
    test_func_err(1);
    test_proc_err();
} /* testErrorFunctions */


#ifdef STANDALONE

#include "Initialize.h"

int main(void)
{
    __initBasicLib();
    testErrorFunctions();
    __deinitBasicLib();
} /* main */

#endif

/* end of TestErrorFunctions.c ... */


