/*
 * Console functions for BASIClib.
 *
 *    Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#include <curses.h>
#include "Console.h"
#include "Initialize.h"
#include "Boolean.h"
#include "Threads.h"
#include "ErrorFunctions.h"


boolean consoleAllowed = true;
boolean inGraphicsMode = false;
WINDOW *cons = NULL;
ThreadLock consoleLock = NULL;


void inline __getConsoleAccess(STATEPARAMS)
{
    if (consoleAllowed == false)
        __runtimeError(STATEARGS, ERR_CANNOT_CONTINUE);
} /* __getConsoleAccess */


void __initConsole(STATEPARAMS)
{
    consoleAllowed = ((__getInitFlags() & INITFLAG_NO_CONSOLE) ? false : true);

    if (consoleAllowed == true)
    {
        __createThreadLock(&consoleLock);
        initscr();
        cons = newwin(0, 0, 0, 0);   /* full screen virtual window. */
        keypad(cons, TRUE)
        (void) nonl();
        (void) cbreak();
        (void) noecho();
        (void) scrollok();
    } /* if */
} /* __initConsole */


void __deinitConsole(STATEPARAMS)
{
    if (consoleAllowed == true)
    {
        endwin();
        __destroyThreadLock(&consoleLock);
    } /* if */
} /* __deinitConsole */


void vbpV_print(STATEPARAMS, PVariant pVar)
{
    PBasicString str;
    int i;
    int max;
    char *data;

    __getConsoleAccess(STATEARGS);

    str = __variantToString(pVar);
    max = str->length;
    *data = str->data;

    __obtainThreadLock(&consoleLock);
    for (i = 0; i < max; i++)
        waddch(cons, data[i]);
    wrefresh(cons);
    __releaseThreadLock(&consoleLock);

    __freeString(str);
} /* vbpV_print */


void vbpii_ViewPrint(STATEPARAMS, int topRow, int bottomRow)
{
    int lines;
    int columns;

    __getConsoleAccess(STATEARGS);

    __obtainThreadLock(&consoleLock);
    getmaxyx(stdscr, lines, columns);

    if ( (topRow < 0) || (bottomRow < topRow) || (bottomRow > lines) )
    {
        __releaseThreadLock(&consoleLock);
        __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
    } /* if */

    else
    {
        wresize(cons, (bottomRow - topRow) + 1, columns);
        mvwin(cons, topRow, 0);
    } /* else */

    __releaseThreadLock(&consoleLock);
} /* vbpii_viewPrint */


void vbp_ViewPrint(STATEPARAMS)
{
    int lines;
    int columns;

    __getConsoleAccess(STATEARGS);

    __obtainThreadLock(&consoleLock);
    getmaxyx(stdscr, lines, columns);
    mvwin(cons, 0, 0);
    wresize(cons, lines, columns);
    __releaseThreadLock(&consoleLock);
} /* vbp_viewPrint */


void vbp_cls(STATEPARAMS)
{
    __getConsoleAccess(STATEARGS);
    __obtainThreadLock(&consoleLock);
    wclear(cons);
    wrefresh(cons);
    __releaseThreadLock(&consoleLock);
} /* vbp_cls */


int vbi_csrline(STATEPARAMS)
{
    int x;
    int y;

    __getConsoleAccess(STATEARGS);
    __obtainThreadLock(&consoleLock);
    getsyx(y, x);
    __releaseThreadLock(&consoleLock);
    return(y);
} /* vbi_csrline */


int vbiV_pos(STATEPARAMS, PVariant pVar)
{
    int x;
    int y;

    /* How retarded to request a variant argument for no reason! */

    __getConsoleAccess(STATEARGS);
    __obtainThreadLock(&consoleLock);
    getsyx(y, x);
    __releaseThreadLock(&consoleLock);
    return(x);
} /* vbiV_pos */

/* color */
/* locate */
/* write */
/* input */
/* line input */
/* INKEY$ */
/* width */
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


/* end of ConsoleFunctions.c ... */

