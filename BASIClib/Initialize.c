/*
 * General initialization routines for BASIClib.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicLib.h"

static __long initFlags = INITFLAG_NOT_INITIALIZED;
static __boolean initialized = false;


void __initBasicLib(void *base, __long flags, int argc, char **argv)
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

        __initMemoryManager(base, NULL);   /* !!! FIX! */
        __initSignalHandlers();
        __initBasicError();
        __initConsoleFunctions();
        __initEnvrFunctions(argc, argv);
        __initTimeDateFunctions();
        __initBasicFileStream();
        __initGUIFrontEnd();
        __initThreads();    /* Make sure this is last init call. */

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
        __deinitEnvrFunctions();
        __deinitThreads();
        __deinitGUIFrontEnd();
        __deinitConsoleFunctions();
        __deinitBasicError();
        __deinitMemoryManager();
        initialized = false;
    } /* if */
} /* __deinitBasicLib */


void __initThread(__integer tidx)
/*
 * This is a entry point to alert modules that a new thread has been spun, 
 *  and to allocate space as necessary. The given index might be recycled
 *  from a dead thread.
 *
 *    params : tidx == new thread's index.
 *   returns : void.
 */
{
    __initThreadMemoryManager(tidx);
    __initThreadBasicError(tidx);
} /* __initThread */


void __deinitThread(__integer tidx)
/*
 * This is a entry point to alert modules that a thread has died, 
 *  and to handle it as necessary. The given index might be recycled
 *  for use with a future thread.
 *
 *    params : tidx == new thread's index.
 *   returns : void.
 */
{
    __deinitThreadMemoryManager(tidx);
} /* __deinitThread */


__long __getInitFlags(void)
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


void __shellOutNotification(void)
/*
 * Call this function before spawning another process. Other modules are
 *  notified from here, so they can do any cleanup they need beforehand.
 *
 *     params : void.
 *    returns : void.
 */
{
    /*__shellOutConsole();*/
} /* __shellOutNotification */


void __shellRetNotification(void)
/*
 * Call this function after returning from a  spawned process. Other modules
 *  are notified from here, so they can do any restoration needed.
 *
 *     params : void.
 *    returns : void.
 */
{
    /*__shellRetConsole();*/
} /* __shellRetNotification */



/* !!! still need: */
/* clear (maybe?) */

/* end of Initialize.c ... */

