/*
 * Console functions for BASIClib.
 *
 *    Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#include <curses.h>
#include "Console.h"
#include "Initialize.h"

boolean noConsole = true;
WINDOW *cons;

void __initConsole(STATEPARAMS)
{
    noConsole = ((__getInitFlags() & INITFLAG_NO_CONSOLE) ? true : false);

    if (noConsole == false)
    {
        cons = initscr();
        if (cons == NULL)
            __runtimeError(STATEARGS, ERR_INTERNAL_ERROR);
    } /* if */
} /* __initConsole */


void __deinitConsole(STATEPARAMS)
{
    if (noConsole == false)
    {
        endwin();
        delscreen(cons);
    } /* if */
} /* __deinitConsole */


/* end of Console.c ... */

