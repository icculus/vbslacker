/*
 * This code is responsible for creating the structure for vbSlacker
 *  GUI support.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "GUIFrontEnd.h"

static __boolean initializedGUIFrontEnd = false;


void __initGUIFrontEnd(void)
/*
 * Initialize the GUI framework, if the program wants to enable it.
 *
 *
 */
{
    if (__getInitFlags() & INITFLAG_ENABLE_GUIFRONTEND)
    {
        initializedGUIFrontEnd = true;
    } /* if */
} /* __initGUIFrontEnd */


void __deinitGUIFrontEnd(void)
/*
 * Deinitialize the GUI framework, if the program initialized it in the
 *  first place.
 *
 *    params : void.
 *   returns : void.
 */
{
    if (initializedGUIFrontEnd)
    {
        /* !!! deinitialize it. */
        initializedGUIFrontEnd = false;
    } /* if */
} /* __deinitGUIFrontEnd */


/* end of GUIFrontEnd.c ... */

