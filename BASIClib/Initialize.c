/*
 * Initialization function. Call __initBasicLib() before doing anything
 *  else with the library. This function just calls each other sections'
 *  initialization function, if one exists.
 *
 *     params : void.
 *    returns : void.
 */

#include "Initialize.h"
#include "OnEvents.h"
#include "TimeDateFunctions.h"

void __initBasicLib(void)
{
    __initOnEvents();
    __initTimeDateFunctions();
} /* __initBasicLib */

void __deinitBasicLib(void)
{
} /* __deinitBasicLib */

/* end of Initialize.c ... */

