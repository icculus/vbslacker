/*
 * General initialization and module communication routines for BASIClib.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include "BasicLib.h"


static __boolean initialized = false;
static __long initFlags = INITFLAG_NOT_INITIALIZED;
static char *vbHomeDir = NULL;


static void __calculateUserAppDir(void)
/*
 * Attempt to find out where the "vbHome" directory should be. By default,
 *  it will be "$HOME/.vbslacker/" ...but, this can be overriden by setting
 *  the environment variable $VBHOME.
 *
 * (Failing all else, the thing gets set to "/.vbslacker/" ...)
 *
 *     params : void.
 *    returns : void.
 */
{
    char *rc;
    char *retVal = NULL;

#warning switch this around, the real code is far from the conditional...
#warning Needs to check if path char is already on envr. var...

    if (vbHomeDir != NULL)     /* safety check. */
        return;

    rc = getenv(__ENVSTR_VBHOME);

    if (rc == NULL)     /* no $VBHOME? Try the default... */
    {
        rc = getenv(__ENVSTR_HOME);

        if (rc == NULL) /* No $HOME?! Go for a REAL default... */
        {
            retVal = malloc(strlen(__DIRNAME_VBHOME) + 3);
            if (retVal != NULL)
            {
                sprintf(retVal, "%c%s%c", __PATHCHAR,
                        __DIRNAME_VBHOME, __PATHCHAR);
            } /* if */
        } /* if */

        else
        {
            retVal = malloc(strlen(rc) + strlen(__DIRNAME_VBHOME) + 2);
            if (retVal != NULL)
            {
                sprintf(retVal, "%s%c%s%c", rc, __PATHCHAR,
                        __DIRNAME_VBHOME, __PATHCHAR);
            } /* if */
        } /* if */
    } /* if */

    else    /* $VBHOME exists? */
    {
        retVal = malloc(strlen(rc) + 2);
        if (retVal != NULL)
            sprintf(retVal, "%s%c", rc, __PATHCHAR);
    } /* else */

    vbHomeDir = retVal;
} /* __calculateUserAppDir */


char *__getUserAppDir(void)
/*
 * Get the directory where vbSlacker's user-specific files can be stored.
 *  (i.e. - "/home/bobo/.vbslacker/") The returned string is allocated
 *  via __memAllocNoPtrs(), and will be automatically garbage collected.
 *  please set all references to NULL when finished with them.
 *
 *     params : void.
 *    returns : ptr to string containing path.
 */
{
    char *retVal = NULL;

    if (vbHomeDir != NULL)
        retVal = __copyAscizString(vbHomeDir);

    return(retVal);
} /* __getUserAppDir */


__boolean __initializationComplete(void)
/*
 * Inform whether BASIClib initialization is complete. If this returns false,
 *  assume nothing but the standard C library has been initialized
 *  fully. Avoid BASIClib calls.
 *
 *      params : void.
 *     returns : (true) if BASIClib is good to go, (false) otherwise.
 */
{
    return(initialized);
} /* __initializationComplete */



void __initBasicLib(__long flags, int argc, char **argv, char **envp)
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

        __calculateUserAppDir();
        __initLanguage(vbHomeDir);

        __initMemoryManager();
        __initSignalHandlers();
        __initBasicError();
        __initConsoleFunctions();
        __initEnvrFunctions(argc, argv);
        __initTimeDateFunctions();
        __initRegistryFunctions();
        __initGUIFrontEnd();
        __initFileSystemFunctions();
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
        initialized = false;
        __deinitFileSystemFunctions();
        __deinitRegistryFunctions();
        __deinitEnvrFunctions();
        __deinitThreads();
        __deinitGUIFrontEnd();
        __deinitConsoleFunctions();
        __deinitBasicError();
        __deinitLanguage();
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
    __initThreadBasicError(tidx);
    __initThreadFileSystemFunctions(tidx);
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
    __deinitThreadFileSystemFunctions(tidx);
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

