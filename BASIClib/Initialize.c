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
#include "Boolean.h"


void __initBasicLib(void)
/*
 * Global initialization function. Call __initBasicLib() before doing anything
 *  else with the library. This function just calls each other sections'
 *  initialization function, if one exists.
 *
 *     params : void.
 *    returns : void.
 */
{
    __initErrorFunctions();
    __initOnEvents();
    __initTimeDateFunctions();
    __initBasicFileStream();
    __initThreads();

    /* !!! register __deinitBasicLib() with atexit()... */
} /* __initBasicLib */


void __deinitBasicLib(void)
/*
 * Call this before exiting a program using BASIClib.
 */
{
    __deinitThreads();
    __deinitOnEvents();
} /* __deinitBasicLib */


void __initThread(int tidx)
/*
 * This is a entry point to alert modules that a new thread has been spun, 
 *  and to allocate space as necessary. The given index might be recycled
 *  from a dead thread.
 *
 *    params : tidx == new thread's index.
 *   returns : void.
 */
{
    __initThreadOnEvents(tidx);
} /* __initThread */


void __deinitThread(int tidx)
/*
 * This is a entry point to alert modules that a thread has died, 
 *  and to handle it as necessary. The given index might be recycled
 *  for use with a future thread.
 *
 *    params : tidx == new thread's index.
 *   returns : void.
 */
{
    __deinitThreadOnEvents(tidx);
} /* __deinitThread */


/* end of Initialize.c ... */

