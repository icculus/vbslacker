/*
 * Mainline for BASIClib testing routines.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "TestLib.h"


void initTestLib(STATEPARAMS, int argc, char **argv)
{
    setbuf(stdout, NULL);
    printf("\n[TESTLIB FOR VBSLACKER STARTING UP...]\n");
    printf("Making call to __initBasicLib()...\n"); 
    __initBasicLib(STATEARGS, INITFLAG_DISABLE_CONSOLE, argc, argv);
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
    testMathFunctions(STATEARGS);
} /* testEverything */


int main(int argc, char **argv)
{
    initTestLib(NULLSTATEARGS, argc, argv);
    testEverything(NULLSTATEARGS);
    deinitTestLib(NULLSTATEARGS);
    return(0);
} /* main */

/* end of TestLib.c ... */

