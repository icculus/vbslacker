/*
 * Console support via the (n)curses library for BASIClib.
 *
 *    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "ConsoleFunctions.h"

#ifdef WIN32   /* cygwin doesn't come with ncurses; won't compile for me. */

#warning WIN32 has no curses support, yet!
__boolean __initCursesConsole(STATEPARAMS) { return(false); }

#else

#include "CursesConsole.h"
#include <string.h>
#include <curses.h>

static WINDOW *cons = NULL;     /* WINDOW structure for printable window. */
static ThreadLock consoleLock;

static int __curs_openConsole(STATEPARAMS)
/*
 * Initialize the curses library, and do all sorts of setup.
 *
 *     params : void.
 *    returns : void.
 */
{
    int retVal = -1;
    initscr();
    start_color();
    cons = newwin(0, 0, 0, 0);   /* full screen virtual window. */
    if (cons != NULL)
    {
        __createThreadLock(STATEARGS, &consoleLock);
        keypad(cons, TRUE);
        (void) nonl();
        (void) cbreak();
        (void) noecho();
        (void) scrollok(cons, TRUE);
        retVal = 1;
    } /* if */

    return(retVal);
} /* __curs_openConsole */


static void __curs_deinitConsoleHandler(STATEPARAMS)
/*
 * Deinitialize the curses library.
 *
 *    params : void.
 *   returns : void.
 */
{
    __obtainThreadLock(STATEARGS, &consoleLock);
    endwin();
    __releaseThreadLock(STATEARGS, &consoleLock);
    __destroyThreadLock(STATEARGS, &consoleLock);
} /* __curs_deinitConsole */


static void __curs_printNChars(STATEPARAMS, char *str, int n)
/*
 * Write (n) chars at (str) to the printable window, scrolling if needed, 
 *  and moving the cursor to the new position.
 *
 *   params : str == string to write.
 *            n == char count to write.
 *  returns : void.
 */
{
    int i;

    __obtainThreadLock(STATEARGS, &consoleLock);
    for (i = 0; i < n; i++)
        waddch(cons, str[i]);
    wrefresh(cons);
    __releaseThreadLock(STATEARGS, &consoleLock);
} /* __curs_printNChars */


static void __curs_printNewLine(STATEPARAMS)
/*
 * Move the cursor down to the start of the next line. Scroll if necessary.
 *
 *    params : void.
 *   returns : void.
 */
{
} /* __curs_printNewLine */


static void __curs_vbpii_viewPrint(STATEPARAMS, int topRow, int bottomRow)
/*
 * Set console lines (top) to (bottom) as the printable window.
 *
 *     params : top    == highest line, option base 1.
 *              bottom == lowest line, option base 1.
 *    returns : void.
 */
{
    int lines;
    int columns;

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
/*
 * Set the whole console window printable.
 *
 *     params : top    == highest line, option base 1.
 *              bottom == lowest line, option base 1.
 *    returns : void.
 */
{
    int lines;
    int columns;

    __obtainThreadLock(STATEARGS, &consoleLock);
    getmaxyx(stdscr, lines, columns);
    mvwin(cons, 0, 0);
    wresize(cons, lines, columns);
    __releaseThreadLock(STATEARGS, &consoleLock);
} /* __curs_vbp_viewPrint */



static void __curs_vbp_cls(STATEPARAMS)
/*
 * Clear the current printable window. The window will be blanked of
 *  characters, and set to the background color. The cursor is moved to
 *  the upper-left hand corner of the printable window.
 *
 *     params : void.
 *    returns : void.
 */
{
    __obtainThreadLock(STATEARGS, &consoleLock);
    wclear(cons);
    wrefresh(cons);
    __releaseThreadLock(STATEARGS, &consoleLock);
} /* vbp_cls */


static int __curs_vbi_csrline(STATEPARAMS)
/*
 * Return current cursor row.
 *
 *    params : void.
 *   returns : current y-coordinate of cursor.
 */
{
    int x;
    int y;

    __obtainThreadLock(STATEARGS, &consoleLock);
    getsyx(y, x);
    __releaseThreadLock(STATEARGS, &consoleLock);
    return(y);
} /* __curs_vbi_csrline */


static int __curs_vbia_pos(STATEPARAMS, void *pVar)
/*
 * Return current cursor column.
 *
 *    params : pVar == ignored, any expression.
 *   returns : current x-coordinate of cursor.
 */
{
    int x;
    int y;

    __obtainThreadLock(STATEARGS, &consoleLock);
    getsyx(y, x);
    __releaseThreadLock(STATEARGS, &consoleLock);
    return(x);
} /* vbiA_pos */


static void __curs_vbpiii_color(STATEPARAMS, int fore, int back, int bord)
/*
 * Set a new printing color.
 *
 *    params : fore == new foreground color of text.
 *             back == new background color of text.
 *             bord == ignored, screen border color.
 *   returns : void.
 */
{
/*    wcolor_set(cons, , NULL); !!! */
} /* __curs_vbpiii_color */


static void __curs_vbpil_color(STATEPARAMS, int fore, long feh)
/*
 * This form of the COLOR command is only for graphics mode, so throw a
 *  runtime error.
 */
{
    __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
} /* __curs_vbpiii_color */


static void __curs_vbpi_color(STATEPARAMS, int fore)
/*
 * This form of the COLOR command is only for graphics mode, so throw a
 *  runtime error.
 */
{
    __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
} /* __curs_vbpiii_color */


static void __curs_getConsoleHandlerName(STATEPARAMS, char *buffer, int size)
/*
 * (Getting rather object-oriented...) copy the name of this console
 *  handler to a buffer.
 *
 *      params : buffer == allocated buffer to copy name to.
 *               size   == maximum bytes to copy to buffer.
 *     returns : void.
 */
{
    strncpy(buffer, "CursesConsole", size);
} /* __curs_getConsoleHandlerName */


__boolean __initCursesConsole(STATEPARAMS)
/*
 * Attempt to initialize curses library access.
 *
 *   params : void.
 *  returns : boolean true if initialized, false on error.
 */
{
    __boolean retVal = false;

    if (__curs_openConsole(STATEARGS) != -1)
    {
        __getConsoleHandlerName = __curs_getConsoleHandlerName;
        __deinitConsoleHandler = __curs_deinitConsoleHandler;
        __printNewLine = __curs_printNewLine;
        __printNChars = __curs_printNChars;
        vbpii_viewPrint = __curs_vbpii_viewPrint;
        vbp_viewPrint = __curs_vbp_viewPrint;
        vbp_cls = __curs_vbp_cls;
        vbi_csrline = __curs_vbi_csrline;
        vbia_pos = __curs_vbia_pos;
        vbpiii_color = __curs_vbpiii_color;
        vbpil_color = __curs_vbpil_color;
        vbpi_color = __curs_vbpi_color;
        retVal = true;
    } /* if */

    return(retVal);
} /* __initCursesConsole */

#endif  /* defined WIN32 */

/* end of CursesConsole.c ... */

