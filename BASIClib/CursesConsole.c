/*
 * Console support via the (n)curses library for BASIClib.
 *
 *    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <curses.h>
#include "ConsoleFunctions.h"
#include "CursesConsole.h"


static WINDOW *cons = NULL;


static int __curs_openConsole(STATEPARAMS)
{
    initscr();
    start_color();
    cons = newwin(0, 0, 0, 0);   /* full screen virtual window. */
    keypad(cons, TRUE);
    (void) nonl();
    (void) cbreak();
    (void) noecho();
    (void) scrollok(cons, TRUE);
    return(cons);
} /* __curs_openConsole */


static void __curs_deinitConsoleHandler(STATEPARAMS)
{
    endwin();
} /* __curs_deinitConsole */


static void __curs_vbpV_print(STATEPARAMS, PVariant pVar)
{
    PBasicString str;
    int i;
    int max;
    char *data;

    __getConsoleAccess(STATEARGS);

    str = __variantToString(STATEARGS, pVar, true);
    max = str->length;
    data = str->data;

    __obtainThreadLock(STATEARGS, &consoleLock);
    for (i = 0; i < max; i++)
        waddch(cons, data[i]);
    wrefresh(cons);
    __releaseThreadLock(STATEARGS, &consoleLock);
} /* __curs_vbpV_print */


static void __curs_vbpii_viewPrint(STATEPARAMS, int topRow, int bottomRow)
{
    int lines;
    int columns;

    __getConsoleAccess(STATEARGS);

    __obtainThreadLock(STATEARGS, &consoleLock);
    getmaxyx(stdscr, lines, columns);

    if ( (topRow < 0) || (bottomRow < topRow) || (bottomRow > lines) )
    {
        __releaseThreadLock(STATEARGS, &consoleLock);
        __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
    } /* if */

    else
    {
        wresize(cons, (bottomRow - topRow) + 1, columns);
        mvwin(cons, topRow, 0);
    } /* else */

    __releaseThreadLock(STATEARGS, &consoleLock);
} /* __curs_vbpii_viewPrint */


static void __curs_vbp_viewPrint(STATEPARAMS)
{
    int lines;
    int columns;

    __getConsoleAccess(STATEARGS);

    __obtainThreadLock(STATEARGS, &consoleLock);
    getmaxyx(stdscr, lines, columns);
    mvwin(cons, 0, 0);
    wresize(cons, lines, columns);
    __releaseThreadLock(STATEARGS, &consoleLock);
} /* __curs_vbp_viewPrint */



static void __curs_vbp_cls(STATEPARAMS)
{
    __getConsoleAccess(STATEARGS);
    __obtainThreadLock(STATEARGS, &consoleLock);
    wclear(cons);
    wrefresh(cons);
    __releaseThreadLock(STATEARGS, &consoleLock);
} /* vbp_cls */


static int __curs_vbi_csrline(STATEPARAMS)
{
    int x;
    int y;

    __getConsoleAccess(STATEARGS);
    __obtainThreadLock(STATEARGS, &consoleLock);
    getsyx(y, x);
    __releaseThreadLock(STATEARGS, &consoleLock);
    return(y);
} /* __curs_vbi_csrline */


static int __curs_vbiA_pos(STATEPARAMS, void *pVar)
{
    int x;
    int y;

    __getConsoleAccess(STATEARGS);
    __obtainThreadLock(STATEARGS, &consoleLock);
    getsyx(y, x);
    __releaseThreadLock(STATEARGS, &consoleLock);
    return(x);
} /* vbiA_pos */


static void __curs_vbpiii_color(STATEPARAMS, int fore, int back, int bord)
{
    wcolor_set(cons, !!!
} /* __curs_vbpiii_color */


static void __curs_vbpil_color(STATEPARAMS, int fore, long feh)
{
    __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
} /* __curs_vbpiii_color */


static void __curs_vbpi_color(STATEPARAMS, int fore)
{
    __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
} /* __curs_vbpiii_color */


static void __curs_getConsoleHandlerName(STATEPARAMS, char *buffer, int size)
{
    strncpy(buffer, "CursesConsole", size);
} /* __curs_getConsoleHandlerName */


boolean __initCursesConsole(STATEPARAMS)
/*
 * Attempt to initialize curses library access.
 *
 *   params : void.
 *  returns : boolean true if initialized, false on error.
 */
{
    boolean retVal = false;

    if (__curs_openConsole(STATEARGS) != -1)
    {
        __getConsoleHandlerName = __cons_getConsoleHandlerName;
        __deinitConsoleHandler = __curs_deinitConsoleHandler;
        vbpS_print = __curs_vbpS_print;
        vbpii_viewPrint = __curs_vbpii_viewPrint;
        vbp_viewPrint = __curs_vbpii_viewPrint;
        vbp_cls = __curs_vbp_cls;
        vbi_csrline = __curs_vbi_csrline;
        vbiA_pos = __curs_vbiA_pos;
        vbpiii_color = __curs_vbpiii_color;
        vbpiii_color = __curs_vbpil_color;
        vbpiii_color = __curs_vbpi_color;
        retVal = true;
    } /* if */
} /* __initCursesConsole */

/* end of CursesConsole.c ... */

