/*
 * Direct console support for BASIClib. Platform specific support for
 *  Linux (and soon, Win32.)
 *
 *    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "ConsoleFunctions.h"
#include "DirectConsole.h"

#define min(x, y) ((x) < (y) ? (x) : (y))

    /* various console attributes... */
static unsigned char x = 0;
static unsigned char y = 0;
static unsigned char lines = 0;
static unsigned char columns = 0;
static int winTop = 0;
static int winBottom = 0;

static ThreadLock consoleLock;


#ifdef LINUX   /* Completely non-portable Linux-specific support. */

static int cons = -1;   /* file handle of /dev/vcsa? device... */
static unsigned char curColor = 7;

#define __xyToConsoleMatrix(x, y)  ((((columns * 2) * y) + (x * 2)) + 4)
#define __consoleMatrixToX(pos) ((pos - 4) % (columns * 2))
#define __consoleMatrixToY(pos) ((pos - 4) / (columns * 2))


static void __setCursorXY(STATEPARAMS, unsigned char _x, unsigned char _y)
/*
 * update our cursor position variables, and /dev/vcsa?'s records.
 *
 *      params : _x, _y == new cursor position.
 *     returns : void.
 */
{
    unsigned char buf[2] = {_x, _y};

    x = _x;
    y = _y;

    __obtainThreadLock(STATEARGS, &consoleLock);
    lseek(cons, 2, SEEK_SET);   /* position in /dev/vcsa? of X cursor. */
    write(cons, buf, 2);        /* update the cursor. */
    __releaseThreadLock(STATEARGS, &consoleLock);
} /* __setCursorXY */


static void __scrollConsole(STATEPARAMS)
/*
 * Move all but the top line of the printable window up one line, and
 *  blank the bottom line.
 *
 *    params : void.
 *   returns : void.
 */
{
    int columnBytes = columns * 2;
    int scrollStart = winTop + (columnBytes);
    int bufSize = winBottom - scrollStart;
    char scrollBuf[bufSize];
    char blankBuf[columnBytes];
    int i;

    for (i = 0; i < columnBytes; i += 2)
    {
        blankBuf[i]     = curColor;
        blankBuf[i + 1] = ' ';
    } /* for */

    __obtainThreadLock(STATEARGS, &consoleLock);
    lseek(cons, scrollStart, SEEK_SET); /* read the console. */
    read(cons, scrollBuf, bufSize);

    lseek(cons, winTop, SEEK_SET);  /* copy it back, one line higher... */
    write(cons, scrollBuf, bufSize);
    write(cons, blankBuf, columnBytes);  /* fill last line with spaces... */
    __releaseThreadLock(STATEARGS, &consoleLock);
} /* __scrollConsole */


static int __cons_openConsole(STATEPARAMS)
/*
 * Open the virtual console for read/write, if at a virtual console, and
 *  the kernel will permit it.
 *
 *    params : void.
 *   returns : false on error, true on success.
 */
{
    char *tty = ttyname(STDOUT_FILENO);
    char consDev[strlen(tty) + 20];
    unsigned char br[4];

    if (tty != NULL)
    {
        if ( (strncmp("/dev/tty", tty, 8) == 0) && (isdigit(tty[8])) )
        {
            strcpy(consDev, "/dev/vcsa");
            strcat(consDev, tty + 8);
            cons = open(consDev, O_RDWR);
            if (cons != -1)
            {
                if (read(cons, &br, 4) != 4)
                {
                    close(cons);
                    cons = -1;
                } /* if */
                else
                {
                    __createThreadLock(STATEARGS, &consoleLock);
                    lines = (int) br[0];
                    columns = (int) br[1];
                    x = (int) br[2];
                    y = (int) br[3];
                    curColor = 7;
                    winTop = 0;
                    winBottom = __xyToConsoleMatrix(columns, lines);
                } /* else */
            } /* if */
        } /* if */
    } /* if */

    return(cons);
} /* __cons_openConsole */


static void __cons_deinitConsoleHandler(STATEPARAMS)
/*
 * Move the cursor to the start of the next line, and close the console
 *  device.
 *
 *     params : void.
 *    returns : void.
 */
{
    if (cons != -1)
    {
        __obtainThreadLock(STATEARGS, &consoleLock);
        __setCursorXY(STATEARGS, 0, y + 1);
        close(cons);
        __releaseThreadLock(STATEARGS, &consoleLock);
        __destroyThreadLock(STATEARGS, &consoleLock);
    } /* if */
} /* __cons_deinitConsole */


static void __cons_vbpS_print(STATEPARAMS, PBasicString pStr)
/*
 * Write a string to the printable window, scrolling if needed, and
 *  moving the cursor to the new position.
 *
 *   params : pStr == BASIC string to write.
 *  returns : void.
 */
{
/*
    int max = pStr->length * 2;
    char data[max];
    int i;
    int n;
    int seekpos = __xyToConsoleMatrix(x, y);
    int bw = 0;
    int bytesInRow = columns * 2;
*/
    


} /* __cons_vbpS_print */


static void __cons_printNewLine(STATEPARAMS)
/*
 * Move the cursor down to the start of the next line. Scroll if necessary.
 *
 *    params : void.
 *   returns : void.
 */
{
    x = 0;
    y++;

    if (__xyToConsoleMatrix(x, y) > winBottom)
    {
        y--;
        __scrollConsole(STATEARGS);
        __setCursorXY(STATEARGS, x, y);
    } /* if */
} /* __cons_printNewLine */


static void __cons_vbp_cls(STATEPARAMS)
/*
 * Clear the current printable window. The window will be blanked of
 *  characters, and set to the background color. The cursor is moved to
 *  the upper-left hand corner of the printable window.
 *
 *     params : void.
 *    returns : void.
 */
{
    int max = winBottom - winTop;
    unsigned char buffer[max];
    int i;

    for (i = 0; i < max; i += 2)
    {
        buffer[i]     = curColor;
        buffer[i + 1] = ' ';
    } /* for */

    __obtainThreadLock(STATEARGS, &consoleLock);
    lseek(cons, winTop, SEEK_SET);
    write(cons, buffer, max);
    __setCursorXY(STATEARGS, __consoleMatrixToX(winTop),
                   __consoleMatrixToY(winTop));
    __releaseThreadLock(STATEARGS, &consoleLock);
} /* __cons_vbp_cls */


static void __cons_vbpiii_color(STATEPARAMS, int fore, int back, int bord)
/*
 * Set a new printing color.
 *
 *    params : fore == new foreground color of text.
 *             back == new background color of text.
 *             bord == ignored, screen border color.
 *   returns : void.
 */
{
} /* __cons_vbpiii_color */

#endif


#ifdef WIN32   /* stubs for now for win32. */
static int  __cons_openConsole(STATEPARAMS) { return(-1); }
static void __cons_deinitConsoleHandler(STATEPARAMS) {}
static void __cons_vbpS_print(STATEPARAMS, PBasicString pStr) {}
static void __cons_vbp_cls(STATEPARAMS) {}
static void __cons_vbpiii_color(STATEPARAMS, int fore, int back, int bord) {}
static void __cons_printNewLine(STATEPARAMS) {}
#endif

static void __cons_vbpii_viewPrint(STATEPARAMS, int top, int bottom)
/*
 * Set console lines (top) to (bottom) as the printable window.
 *
 *     params : top    == highest line, option base 1.
 *              bottom == lowest line, option base 1.
 *    returns : void.
 */
{
    top--;      /* Make parameters option base 0. */
    bottom--;

    if ( (top < 0) || (bottom < top) || (bottom > lines) )
        __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
    else
    {
        __obtainThreadLock(STATEARGS, &consoleLock);
        winTop = __xyToConsoleMatrix(0, top);
        winBottom = __xyToConsoleMatrix(columns, bottom);
        __setCursorXY(STATEARGS, 0, top);
        __releaseThreadLock(STATEARGS, &consoleLock);
    } /* else */
} /* __cons_vbpii_viewPrint */


static void __cons_vbp_viewPrint(STATEPARAMS)
/*
 * Set the whole console window printable.
 *
 *     params : void.
 *    returns : void.
 */
{
    __obtainThreadLock(STATEARGS, &consoleLock);
    winTop = __xyToConsoleMatrix(0, 0);
    winBottom = __xyToConsoleMatrix(columns, lines);
    __setCursorXY(STATEARGS, 0, 0);
    __releaseThreadLock(STATEARGS, &consoleLock);
} /* __cons_vbp_viewPrint */



static int __cons_vbi_csrline(STATEPARAMS)
/*
 * Return current cursor row.
 *
 *    params : void.
 *   returns : current y-coordinate of cursor.
 */
{
    return(y);
} /* __cons_vbi_csrline */


static int __cons_vbia_pos(STATEPARAMS, void *pVar)
/*
 * Return current cursor column.
 *
 *    params : pVar == ignored, any expression.
 *   returns : current x-coordinate of cursor.
 */
{
    return(x);
} /* __cons_vbia_pos */


static void __cons_vbpil_color(STATEPARAMS, int fore, long feh)
/*
 * This form of the COLOR command is only for graphics mode, so throw a
 *  runtime error.
 */
{
    __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
} /* __cons_vbpiii_color */


static void __cons_vbpi_color(STATEPARAMS, int fore)
/*
 * This form of the COLOR command is only for graphics mode, so throw a
 *  runtime error.
 */
{
    __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
} /* __cons_vbpiii_color */


static void __cons_getConsoleHandlerName(STATEPARAMS, char *buffer, int size)
/*
 * (Getting rather object-oriented...) copy the name of this console
 *  handler to a buffer.
 *
 *      params : buffer == allocated buffer to copy name to.
 *               size   == maximum bytes to copy to buffer.
 *     returns : void.
 */
{
    strncpy(buffer, "DirectConsole", size);
} /* __cons_getConsoleHandlerName */


boolean __initDirectConsole(STATEPARAMS)
/*
 * Attempt to initialize direct-to-console access.
 *
 *   params : void.
 *  returns : boolean true if initialized, false on error.
 */
{
    boolean retVal = false;

    if (__cons_openConsole(STATEARGS) != -1)
    {
        __deinitConsoleHandler = __cons_deinitConsoleHandler;
        __getConsoleHandlerName = __cons_getConsoleHandlerName;
        __printNewLine = __cons_printNewLine;
        vbpS_print = __cons_vbpS_print;
        vbpii_viewPrint = __cons_vbpii_viewPrint;
        vbp_viewPrint = __cons_vbp_viewPrint;
        vbp_cls = __cons_vbp_cls;
        vbi_csrline = __cons_vbi_csrline;
        vbia_pos = __cons_vbia_pos;
        vbpiii_color = __cons_vbpiii_color;
        vbpil_color = __cons_vbpil_color;
        vbpi_color = __cons_vbpi_color;
        retVal = true;
    } /* if */

    return(retVal);
} /* __initDirectConsole */

/* end of DirectConsole.c ... */

