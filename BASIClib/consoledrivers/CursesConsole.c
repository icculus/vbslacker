/*
 * Console support via the (n)curses library for BASIClib.
 *
 *    Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "StdBasic.h"

#ifdef NOCURSES
__boolean __initCursesConsole(void) { return(false); }

#else

#include <string.h>
#include <curses.h>

static void __curs_deinitConsoleDriver(void);
static void __curs_printNChars(const __byte *str, __long n);
static void __curs_printNewLine(void);
static void __curs_vbpii_viewPrint(__integer topRow, __integer botmRow);
static void __curs_vbp_viewPrint(void);
static void __curs_vbp_cls(void);
static __integer __curs_vbi_csrline(void);
static __integer __curs_vbia_pos(void *pVar);
static void __curs_vbpiii_color(__integer fore,
                                 __integer back,
                                 __integer bord);
static void __curs_vbpil_color(__integer fore, __long feh);
static void __curs_vbpi_color(__integer fore);
static void __curs_vbpii_locate(__integer newY, __integer newX);
static void __curs_vbpNi_locate(__integer newX);
static void __curs_vbpiN_locate(__integer newY);
static void __curs_vbp_locate(void);
static void __curs_vbpif_sound(__integer frequency, __single duration);
static void __curs_vbp_beep(void);
static void __curs_getConsoleDriverName(__byte *buffer, __integer size);

static __vbConsoleEntries cursesConEntries =
{
    __curs_getConsoleDriverName,
    __curs_deinitConsoleDriver,
    __curs_printNewLine,
    __curs_printNChars,
    __curs_vbpii_viewPrint,
    __curs_vbp_viewPrint,
    __curs_vbp_cls,
    __curs_vbi_csrline,
    __curs_vbia_pos,
    __curs_vbpiii_color,
    __curs_vbpil_color,
    __curs_vbpi_color,
    __curs_vbpii_locate,
    __curs_vbpNi_locate,
    __curs_vbpiN_locate,
    __curs_vbp_locate,
    __curs_vbpif_sound,
    __curs_vbp_beep
};

static WINDOW *cons = NULL;     /* WINDOW structure for printable window. */
static ThreadLock consoleLock;

static __boolean __curs_openConsole(void)
/*
 * Initialize the curses library, and do all sorts of setup.
 *
 *     params : void.
 *    returns : void.
 */
{
    __integer retVal = false;

    initscr();
    start_color();
    cons = newwin(0, 0, 0, 0);   /* full screen virtual window. */
    if (cons != NULL)
    {
        __createThreadLock(&consoleLock);
        keypad(cons, TRUE);
        (void) nonl();
        (void) cbreak();
        (void) noecho();
        (void) scrollok(cons, TRUE);
        retVal = true;
    } /* if */

    return(retVal);
} /* __curs_openConsole */


static void __curs_deinitConsoleDriver(void)
/*
 * Deinitialize the curses library.
 *
 *    params : void.
 *   returns : void.
 */
{
    __obtainThreadLock(&consoleLock);
    endwin();
    __releaseThreadLock(&consoleLock);
    __destroyThreadLock(&consoleLock);
} /* __curs_deinitConsole */


static void __curs_printNChars(const __byte *str, __long n)
/*
 * Write (n) chars at (str) to the printable window, scrolling if needed, 
 *  and moving the cursor to the new position.
 *
 *   params : str == string to write.
 *            n == char count to write.
 *  returns : void.
 */
{
    __long i;

    __obtainThreadLock(&consoleLock);
    for (i = 0; i < n; i++)
        waddch(cons, str[i]);
    wrefresh(cons);
    __releaseThreadLock(&consoleLock);
} /* __curs_printNChars */


static void __curs_printNewLine(void)
/*
 * Move the cursor down to the start of the next line. Scroll if necessary.
 *
 *    params : void.
 *   returns : void.
 */
{
    __obtainThreadLock(&consoleLock);
    waddch(cons, '\n');
    wrefresh(cons);
    __releaseThreadLock(&consoleLock);
} /* __curs_printNewLine */


static void __curs_vbpii_viewPrint(__integer topRow, __integer bottomRow)
/*
 * Set console lines (top) to (bottom) as the printable window.
 *
 *     params : top    == highest line, option base 1.
 *              bottom == lowest line, option base 1.
 *    returns : void.
 */
{
    __integer lines;
    __integer columns;

    __obtainThreadLock(&consoleLock);
    getmaxyx(stdscr, lines, columns);

    if ( (topRow < 0) || (bottomRow < topRow) || (bottomRow > lines) )
    {
        __releaseThreadLock(&consoleLock);
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    } /* if */

    else
    {
        wresize(cons, (bottomRow - topRow) + 1, columns);
        mvwin(cons, topRow - 1, 0);
        wmove(cons, 0, 0);
    } /* else */

    __releaseThreadLock(&consoleLock);
} /* __curs_vbpii_viewPrint */


static void __curs_vbp_viewPrint(void)
/*
 * Set the whole console window printable.
 *
 *     params : top    == highest line, option base 1.
 *              bottom == lowest line, option base 1.
 *    returns : void.
 */
{
    __integer lines;
    __integer columns;

    __obtainThreadLock(&consoleLock);
    getmaxyx(stdscr, lines, columns);
    mvwin(cons, 0, 0);
    wresize(cons, lines, columns);
    wmove(cons, 0, 0);
    __releaseThreadLock(&consoleLock);
} /* __curs_vbp_viewPrint */


static void __curs_vbp_cls(void)
/*
 * Clear the current printable window. The window will be blanked of
 *  characters, and set to the background color. The cursor is moved to
 *  the upper-left hand corner of the printable window.
 *
 *     params : void.
 *    returns : void.
 */
{
    __obtainThreadLock(&consoleLock);
    wclear(cons);
    wrefresh(cons);
    __releaseThreadLock(&consoleLock);
} /* vbp_cls */


static __integer __curs_vbi_csrline(void)
/*
 * Return current cursor row.
 *
 *    params : void.
 *   returns : current y-coordinate of cursor.
 */
{
    int x;
    int y;

    __obtainThreadLock(&consoleLock);
    getyx(cons, y, x);
    __releaseThreadLock(&consoleLock);

    return((__integer) y);
} /* __curs_vbi_csrline */


static __integer __curs_vbia_pos(__attribute__ ((unused)) void *pVar)
/*
 * Return current cursor column.
 *
 *    params : pVar == ignored, any expression.
 *   returns : current x-coordinate of cursor.
 */
{
    int x;
    int y;

    __obtainThreadLock(&consoleLock);
    getyx(cons, y, x);
    __releaseThreadLock(&consoleLock);

    return((__integer) x);
} /* vbiA_pos */


static void __curs_vbpiii_color(__attribute__ ((unused)) __integer fore,
                                __attribute__ ((unused)) __integer back,
                                __attribute__ ((unused)) __integer bord)
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


static void __curs_vbpil_color(__attribute__ ((unused)) __integer fore,
                               __attribute__ ((unused))__long feh)
/*
 * This form of the COLOR command is only for graphics mode, so throw a
 *  runtime error.
 */
{
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
} /* __curs_vbpiii_color */


static void __curs_vbpi_color( __attribute__ ((unused)) __integer fore)
/*
 * This form of the COLOR command is only for graphics mode, so throw a
 *  runtime error.
 */
{
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
} /* __curs_vbpiii_color */


static void __curs_vbpii_locate(__integer newY, __integer newX)
{
    int top;
    int bottom;
    int right;

    getbegyx(cons, top, right);
    getmaxyx(cons, bottom, right);

    if ((newY < top) || (newY > bottom) || (newX < 0) || (newX > right))
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);

    wmove(cons, newY - 1, newX - 1);
    wrefresh(cons);
} /* __curs_vbpii_locate */


static void __curs_vbpNi_locate(__integer newX)
{
    int dummy;
    int right;
    int curY;

    getyx(cons, curY, dummy);
    getmaxyx(cons, dummy, right);

    if ((newX < 0) || (newX > right))
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    wmove(cons, curY, newX - 1);
    wrefresh(cons);
} /* __curs_vbpNi_locate */


static void __curs_vbpiN_locate(__integer newY)
{
    int dummy;
    int top;
    int bottom;
    int curX;

    getyx(cons, dummy, curX);
    getbegyx(cons, top, dummy);
    getmaxyx(cons, bottom, dummy);

    if ((newY < top) || (newY > bottom))
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    wmove(cons, newY - 1, curX);
    wrefresh(cons);
} /* __curs_vbpiN_locate */


static void __curs_vbp_locate(void)
{
} /* __curs_vbp_locate */


static void __curs_vbpif_sound(__attribute__ ((unused)) __integer frequency,
                               __attribute__ ((unused)) __single duration)
{
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
} /* __curs_vbpif_sound */


static void __curs_vbp_beep(void)
{
    beep();     /* fun, huh? */
} /* __curs_vbp_beep */


static void __curs_getConsoleDriverName(__byte *buffer, __integer size)
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
} /* __curs_getConsoleDriverName */


__boolean __initCursesConsole(__vbConsoleEntries *entries)
/*
 * Attempt to initialize curses library access.
 *
 *   params : void.
 *  returns : boolean true if initialized, false on error.
 */
{
    __boolean retVal = __curs_openConsole();
    if (retVal == true)
        memcpy(entries, &cursesConEntries, sizeof (__vbConsoleEntries));

    return(retVal);
} /* __initCursesConsole */

#endif  /* (!)defined NOCURSES */

/* end of CursesConsole.c ... */

