/*
 * Process-related functions for BASIClib.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdlib.h>
#include <time.h>
#include "ProcessFunctions.h"


void _vbpS_shell(PBasicString shCmd)
/*
 * Spawn a shell, and give it (shCmd) as a command line. This can be
 *  used to execute other programs, or use internal shell commands.
 *  Note that BASIC has no defined means to determine what the shell
 *  to use will be, as it has grown up on COMMAND.COM-specific systems.
 *
 * On Unix systems, the shell could be...uh, anything, really. Under
 *  Windows and friends, you have a more standard system, but it shouldn't
 *  matter too much unless you REALLY needed some internal shell command;
 *  programs themselves should execute the same regardless of shell. We hope.
 *
 *      params : shCmd == BASIC string containing command to pass to shell.
 *     returns : void.
 */
{
    char *cmd = __basicStringToAsciz(shCmd);

    __shellOutNotification();
    system(cmd);
    __shellRetNotification();
    __memFree(cmd);
} /* _vbpS_shell */


void _vbp_end(void)
/*
 * Terminate the program.
 *
 *    params : void.
 *   returns : never returns.
 */
{
    exit(0);
} /* _vbp_end */


void _vbp_sleep(void)
/*
 * Make program (ALL THREADS) sleep (yield) until an
 *  OnEvent occurs, or A KEY IS PRESSED. Yikes.
 *
  *  This is basically useless in a multithreaded program.
 *
 *     params : void.
 *    returns : void.
 */
{
#warning vbp_sleep() is a stub!
} /* _vbp_sleep */


void _vbpl_sleep(__long napTime)
/*
 * Make current thread sleep (yield) for (napTime) seconds.
 *
 * BASIC docs say that if (napTime) is zero, sleep continues until an
 *  OnEvent occurs, or A KEY IS PRESSED. Yikes.
 *
 * Sleep will be no less than (napTime), unless interrupted, but there is
 *  no guarantee as to how much longer than naptime the program will sleep.
 *  Such is the burden of multitasking.
 *
 *     params : napTime == seconds to sleep.
 *    returns : void.
 */
{
    time_t startTime;
    time_t currentTime;
    double dNapTime = (double) napTime;

    if (napTime == 0)           /* delegate this to no-arg version ... */
        _vbp_sleep();
    else
    {
        if (time(&startTime) == -1)     /* calendar time unavailable ?! */
            __runtimeError(ERR_INTERNAL_ERROR);
        else
        {
            do      /* give up time slices until (napTime) seconds elaspe. */
            {
                __memDoPartialCollect();
                __threadTimeslice;
                time(&currentTime);
            } while (difftime(currentTime, startTime) < dNapTime);
        } /* else */
    } /* else */
} /* _vbpl_sleep */

/* end of ProcessFunctions.c ... */

