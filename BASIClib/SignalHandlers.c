/*
 * glibc signal handlers.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <signal.h>
#include "SignalHandlers.h"

#warning What to do about SIGWINCH?

static void __crashHandler(int sig)
/*
 * This is called when the program is to be terminated due to a
 *  SIGSEGV, SIGFPE, SIGBUS, etc...While poorly written BASIC
 *  programs should throw runtime errors and exit gracefully,
 *  they should never raise exceptions, so the kernel needs to
 *  terminate them forcefully. Therefore, if such an event occurs,
 *  we collect all the pertinent data we can, and try to send it
 *  on to the BASIClib development team, so such problems (which
 *  may be bugs in BASIClib, or poorly written code doing unexpected
 *  things) can be eliminated.
 *
 */
{
    #warning write __crashHandler() !!!
    printf("__crashHandler() called...\n");
    exit(0);        /* ...and bail. */
} /* __crashHandler */


#ifdef WANTSIGWINCH
static void __sigwinchHandler(int sig)
{
    printf("sigwinch handler!\n");
} /* __sigwinchHandler */
#endif


static void __sigintHandler(int sig)
/*
 * SIGINT is generated when CTRL-C is hit.
 */
{
    __printAsciz("__sigintHandler() called...");
    __printNewLine();
    exit(0);  /* This makes sure __deinitBasicLib() is called. */
} /* __sigintHandler */


void __initSignalHandlers(void)
{
    (void) signal(SIGINT, __sigintHandler);
    (void) signal(SIGSEGV, __crashHandler);
    (void) signal(SIGILL, __crashHandler);
    (void) signal(SIGBUS, __crashHandler);
    (void) signal(SIGFPE, __crashHandler);

#ifdef WANTSIGWINCH
    (void) signal(SIGWINCH, __sigwinchHandler);
#endif
} /* __initSignalHandlers */

/* end of SignalHandlers.c ... */

