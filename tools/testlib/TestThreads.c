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

    printf("Testing __spinThread()...\n");

    changed = false;

    tidx = __spinThread((void *) threadFunc, NULL);
    if (tidx == -1)
        printf("  - Couldn't spin thread. retVal == -1.\n");
    else
    {
        __waitForThreadToDie(tidx);
        if (changed != true)
            printf("  - Thread didn't run, or didn't change flag.\n");
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

#include "Initialize.h"

int main(void)
{
    __initBasicLib();
    testThreads();
    __deinitBasicLib();
} /* main */

#endif

/* end of TestThreads.c ... */


