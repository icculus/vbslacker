/*
 * Mainline for BASIClib testing routines.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "TestLib.h"


void initTestLib(STATEPARAMS)
{
    setbuf(stdout, NULL);
    printf("\n[TESTLIB FOR VBSLACKER STARTING UP...]\n");
    printf("Making call to __initBasicLib()...\n"); 
    __initBasicLib(STATEARGS, INITFLAG_DISABLE_CONSOLE);
} /* initTestLib */

void deinitTestLib(STATEPARAMS)
{
    printf("\n[TESTLIB FOR VBSLACKER SHUTTING DOWN...]\n");
    printf("Making call to __deinitBasicLib()...\n"); 

        /* !!! This will be done automatically, eventually... */
    __deinitBasicLib();

    printf("\n");
} /* deinitTestLib */

void testEverything(STATEPARAMS)
{
    testOnEvents(STATEARGS);
    testErrorFunctions(STATEARGS);
    testThreads(STATEARGS);
    testStringFunctions(STATEARGS);
    testConversionFunctions(STATEARGS);
    testFileIOFunctions(STATEARGS);
} /* testEverything */


int main(void)
{
    initTestLib(NULLSTATEARGS);
    testEverything(NULLSTATEARGS);
    deinitTestLib(NULLSTATEARGS);
    return(0);
} /* main */

/* end of TestLib.c ... */

