/*
 * General initialization routines for BASIClib.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicLib.h"

static long initFlags = INITFLAG_NOT_INITIALIZED;
static boolean initialized = false;


void __initBasicLib(STATEPARAMS, long flags)
/*
 * Global initialization function. Call __initBasicLib() before doing anything
 *  else with the library. This function just calls each other sections'
 *  initialization function, if one exists.
 *
 *     params : void.
 *    returns : void.
 */
{
    if (initialized == false)
    {
        initFlags = flags;

        __initSignalHandlers(STATEARGS);
        __initInternalMemManager(STATEARGS);
        __initErrorFunctions(STATEARGS);
        __initOnEvents(STATEARGS);
        __initConsole(STATEARGS);
        __initTimeDateFunctions(STATEARGS);
        __initBasicFileStream(STATEARGS);

        __initThreads(STATEARGS);    /* Make sure this is last init call. */

        atexit(__deinitBasicLib);

        initialized = true;
    } /* if */
} /* __initBasicLib */


void __deinitBasicLib(void)
/*
 * Call this before exiting a program using BASIClib.
 */
{
    if (initialized == true)
    {
        initFlags = INITFLAG_NOT_INITIALIZED;
        __deinitThreads(NULLSTATEARGS);
        __deinitConsole(NULLSTATEARGS);
        __deinitOnEvents(NULLSTATEARGS);
        __deinitInternalMemManager(NULLSTATEARGS);
        initialized = false;
    } /* if */
} /* __deinitBasicLib */


void __initThread(STATEPARAMS, int tidx)
/*
 * This is a entry point to alert modules that a new thread has been spun, 
 *  and to allocate space as necessary. The given index might be recycled
 *  from a dead thread.
 *
 *    params : tidx == new thread's index.
 *   returns : void.
 */
{
    __initThreadOnEvents(STATEARGS, tidx);
} /* __initThread */


void __deinitThread(STATEPARAMS, int tidx)
/*
 * This is a entry point to alert modules that a thread has died, 
 *  and to handle it as necessary. The given index might be recycled
 *  for use with a future thread.
 *
 *    params : tidx == new thread's index.
 *   returns : void.
 */
{
    __deinitThreadOnEvents(STATEARGS, tidx);
} /* __deinitThread */


long __getInitFlags(STATEPARAMS)
/*
 * This function allows code to get the value of the (initFlags) variable,
 *  while protecting it from modification.
 *
 *    params : void.
 *   returns : current value of (initFlags).
 */
{
    return(initFlags);
} /* __getInitFlags */


void __shellOutNotification(STATEPARAMS)
/*
 * Call this function before spawning another process. Other modules are
 *  notified from here, so they can do any cleanup they need beforehand.
 *
 *     params : void.
 *    returns : void.
 */
{
    /*__shellOutConsole(STATEARGS);*/
} /* __shellOutNotification */


void __shellRetNotification(STATEPARAMS)
/*
 * Call this function after returning from a  spawned process. Other modules
 *  are notified from here, so they can do any restoration needed.
 *
 *     params : void.
 *    returns : void.
 */
{
    /*__shellRetConsole(STATEARGS);*/
} /* __shellRetNotification */



/* !!! still need: */
/* clear (maybe?) */

/* end of Initialize.c ... */

