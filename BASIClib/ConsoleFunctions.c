/*
 * Console functions for BASIClib.
 *
 *    Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#include <curses.h>
#include "Console.h"
#include "Initialize.h"
#include "Boolean.h"

boolean noConsole = true;
WINDOW *cons = NULL;

void __initConsole(STATEPARAMS)
{
    noConsole = ((__getInitFlags() & INITFLAG_NO_CONSOLE) ? true : false);

    if (noConsole == false)
    {
        cons = initscr();
        keypad(cons, TRUE)
        (void) nonl();
        (void) cbreak();
        (void) noecho();
        (void) scrollok();
    } /* if */
} /* __initConsole */


void __deinitConsole(STATEPARAMS)
{
    if (noConsole == false)
        endwin();
} /* __deinitConsole */


void print(STATEPARAMS, PVariant pVar)
{
    PBasicString str = __variantToString(pVar);
    int i;
    int max = str->length;
    char *data = str->data;

    for (i = 0; i < max; i++)
        waddch(cons, data[i]);

    wrefresh(cons);
    __freeString(str);
} /* print */

/* view print x to y */
/* cls */
/* color */
/* locate */
/* write */
/* input */
/* line input */
/* INKEY$ */
/* width */
/* csrline */
/* pos */
/* key */
/* print using */
/* tab */

/*  -- graphics ... -- */
/* circle */
/* draw */
/* get */
/* put */
/* screen */
/* line */
/* paint */
/* point */
/* palette */
/* pcopy */
/* pset */
/* preset */
/* pmap */
/* view */
/* window */


/* end of Console.c ... */

