/*
 * General initialization routines for BASIClib.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "Initialize.h"
#include "Threads.h"
#include "OnEvents.h"
#include "TimeDateFunctions.h"
#include "BasicFileStream.h"
#include "ErrorFunctions.h"
#include "InternalMemManager.h"
#include "Boolean.h"

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
        __initInternalMemManager(STATEARGS);
        __initErrorFunctions(STATEARGS);
        __initOnEvents(STATEARGS);
        __initTimeDateFunctions(STATEARGS);
        __initBasicFileStream(STATEARGS);

        __initThreads(STATEARGS);    /* Make sure this is last init call. */

        /* !!! register __deinitBasicLib() with atexit()... */

        initialized = true;
    } /* if */
} /* __initBasicLib */


void __deinitBasicLib(STATEPARAMS)
/*
 * Call this before exiting a program using BASIClib.
 */
{
    if (initialized == true)
    {
        initFlags = INITFLAG_NOT_INITIALIZED;
        __deinitThreads(STATEARGS);
        __deinitOnEvents(STATEARGS);
        __deinitInternalMemManager(STATEARGS);
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
{
    return(initFlags);
} /* __getInitFlags */

/* end of Initialize.c ... */

