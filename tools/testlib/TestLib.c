/*
 * Mainline for BASIClib testing routines.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "Initialize.h"
#include "TestLib.h"


void initTestLib(void)
{
    setbuf(stdout, NULL);
    printf("\n[TESTLIB FOR VBSLACKER STARTING UP...]\n");
    printf("Making call to __initBasicLib()...\n"); 
    __initBasicLib();
} /* initTestLib */

void deinitTestLib(void)
{
    printf("\n[TESTLIB FOR VBSLACKER SHUTTING DOWN...]\n");
    printf("Making call to __deinitBasicLib()...\n"); 

        /* !!! This will be done automatically, eventually... */
    __deinitBasicLib();
} /* deinitTestLib */


int main(void)
{
    initTestLib();

    testOnEvents();
    testErrorFunctions();
    testThreads();
    testStringFunctions();
    testConversionFunctions();
    testFileIOFunctions();

    deinitTestLib();
    return(0);
} /* main */

/* end of TestLib.c ... */

