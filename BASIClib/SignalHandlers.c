/*
 * glibc signal handlers.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <signal.h>

#warning Get around to writing the signal handlers!

static void __sigintHandler(int sig)
{
    printf("sigint handler!\n");
} /* __sigintHandler */


static void __crashHandler(int sig)
{
    printf("crash handler!\n");
} /* __crashHandler */


static void __sigwinchHandler(int sig)
{
    printf("sigwinch handler!\n");
} /* __sigwinchHandler */


void __initSignalHandlers(STATEPARAMS)
{
    (void) signal(SIGINT, __sigintHandler);
    (void) signal(SIGSEGV, __crashHandler);
    (void) signal(SIGILL, __crashHandler);
    (void) signal(SIGBUS, __crashHandler);
    (void) signal(SIGFPE, __crashHandler);
    (void) signal(SIGWINCH, __sigwinchHandler);
} /* __initSignalHandlers */


/* end of SignalHandlers.c ... */

