/*
 * Test some thread functions...
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include "Threads.h"
#include "Boolean.h"

static boolean changed = false;


void threadFunc(STATEPARAMS, void *args)
/*
 * This sets (changed), and returns. Test that variable to see if
 *  this function ever ran.
 */
{
    changed = true;
} /* threadFunc */


void test__spinThread(STATEPARAMS)
/*
 * Test __spinThread() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    pthread_t tid;
    int tidx;
    int i;

    printf("Testing __spinThread()...\n");

    changed = false;

    tidx = __spinThread(STATEARGS, (void *) threadFunc, NULL);
    if (tidx == -1)
        printf("  - Couldn't spin thread. retVal == -1.\n");
    else
    {
        for (i = 0; i < 10000; i++)
            __threadTimeslice(STATEARGS);
        __waitForThreadToDie(STATEARGS, tidx);
        if (changed != true)
            printf("  - Thread didn't run, or didn't change flag.\n");
    } /* else */
} /* test__spinThread */


void testThreads(STATEPARAMS)
/*
 * This code tests all testable thread functions in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n[TESTING THREAD FUNCTIONS...]\n");

    test__spinThread(STATEARGS);

    /* !!! needs more testing. */

} /* testThreads */


#ifdef STANDALONE

#include "Initialize.h"

int main(void)
{
    __initBasicLib(NULLSTATEARGS);
    testThreads(NULLSTATEARGS);
    __deinitBasicLib(NULLSTATEARGS);
    return(0);
} /* main */

#endif

/* end of TestThreads.c ... */


