/*
 * glibc signal handlers.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <signal.h>

static void __sigintHandler(int sig)
{
    printf("sigint handler!\n");
} /* __sigintHandler */


static void __crashHandler(int sig)
{
    printf("crash handler!\n");
} /* __crashHandler */


void __initSignalHandlers(STATEPARAMS)
{
    (void) signal(SIGINT, __sigintHandler);
    (void) signal(SIGSEGV, __crashHandler);
    (void) signal(SIGILL, __crashHandler);
    (void) signal(SIGBUS, __crashHandler);
    (void) signal(SIGFPE, __crashHandler);
} /* __initSignalHandlers */


/* end of SignalHandlers.c ... */
