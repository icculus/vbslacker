/*
 * Test some thread functions...
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include "BasicLib.h"


extern long warnings;
extern long errors;


static __boolean changed = false;


void threadFunc(void *args)
/*
 * This sets (changed), and returns. Test that variable to see if
 *  this function ever ran.
 */
{
    changed = true;
} /* threadFunc */


void test__spinThread(void)
/*
 * Test __spinThread() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    int tidx;
    int i;

    printf("Testing __spinThread()...\n");

    changed = false;

    tidx = __spinThread((void *) threadFunc, NULL);
    if (tidx == -1)
    {
        warnings++;
        printf("  - Couldn't spin thread. retVal == -1.\n"
               "  -  (If this is a single-threaded app, this is normal.)\n");
    } /* if */
    else
    {
        for (i = 0; i < 10000; i++)
            __threadTimeslice();
        __waitForThreadToDie(tidx);
        if (changed != true)
        {
            printf("  - Thread didn't run, or didn't change flag.\n");
            errors++;
        } /* if */
    } /* else */
} /* test__spinThread */


void testThreads(void)
/*
 * This code tests all testable thread functions in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n[TESTING THREAD FUNCTIONS...]\n");

    test__spinThread();

    /* !!! needs more testing. */

} /* testThreads */


#ifdef STANDALONE

long errors = 0;
long warnings = 0;

int main(int argc, char **argv)
{
    __initBasicLib(INITFLAG_DISABLE_CONSOLE, argc, argv);
    testThreads();
    return(0);
} /* main */

#endif

/* end of TestThreads.c ... */


