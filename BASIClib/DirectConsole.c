/*
 * Direct console support for BASIClib. Platform specific support for
 *  Linux (and soon, Win32.)
 *
 *    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#warning DirectConsole.c needs ThreadLocks!

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


#ifdef LINUX   /* Completely non-portable Linux-specific support. */

static int cons = -1;   /* file handle of /dev/vcsa? device... */
static unsigned char curColor = 7;

#define __xyToConsoleMatrix(x, y)  ((((columns * 2) * y) + (x * 2)) + 4)

static void __setConsoleXY(unsigned char _x, unsigned char _y)
{
    unsigned char buf[2] = {_x, _y};

    x = _x;
    y = _y;

    lseek(cons, 2, SEEK_SET);   /* position in /dev/vcsa? of X cursor. */
    write(cons, buf, 2);        /* update the cursor. */
} /* __setConsoleXY */


static void __scrollConsole(STATEPARAMS)
{
    int columnBytes = columns * 2;
    int scrollStart = winTop + (columnBytes);
    int bufSize = winBottom - scrollStart;
    char scrollBuf[bufSize];
    char blankBuf[columnBytes];

    for (i = 0; i < columnBytes; i += 2)
    {
        blankBuf[i]     = curColor;
        blankBuf[i + 1] = ' ';
    } /* for */

    lseek(cons, scrollStart, SEEK_SET); /* read the console. */
    read(cons, scrollBuf, bufSize);

    lseek(cons, winTop, SEEK_SET);  /* copy it back, one line higher... */
    write(cons, scrollBuf, bufSize);
    write(cons, blankBuf, columnBytes);  /* fill last line with spaces... */
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
{
    if (cons != -1)
    {
        __setCursorXY(0, y + 1);
        close(cons);
    } /* if */
} /* __cons_deinitConsole */


static void __cons_vbpS_print(STATEPARAMS, PBasicString pStr)
{
    int max = pStr->length * 2;
    char data[max * 2];
    int i;
    int n;
    int seekpos = __xyToConsoleMatrix(x, y);
    int bw = 0;
    int bytesInRow = columns * 2;

    for (i = n = 0; i < max; i++, n += 2)
    {
        data[n]     = curColor;
        data[n + 1] = pStr->data[i];
    } /* for */

    max *= 2;  /* actual byte count to write. */

    lseek(cons, seekpos, SEEK_SET);  /* seek to first write position. */
    bw = bottomRow - seekpos;   /* first write: to bottom of window. */
    if (bw > max)               /* won't touch bottom? */
    {
        bw = max;
        x = __consoleMatrixToX(seekpos + max);
        y = __consoleMatrixToY(seekpos + max);
    } /* if */
    else
        y = lines;

    write(cons, data, bw);

    while (bw < max)        /* until all is written. */
    {
        __scrollConsole(STATEARGS);
        x = min(bytesInRow, max - bw);
        write(cons, data + bw, x);
        bw += x;
    } /* while */

    __setConsoleXY(x, y);
} /* __cons_vbpS_print */


static void __cons_vbp_cls(STATEPARAMS)
{
    int max = winBottom - winTop;
    unsigned char buffer[max];
    int i;

    for (i = 0; i < max; i += 2)
    {
        buffer[i]     = curColor;
        buffer[i + 1] = ' ';
    } /* for */

    lseek(cons, winTop, SEEK_SET);
    write(cons, buffer, max);

    __setConsoleXY(__consoleMatrixToX(winTop), __consoleMatrixToY(winTop));
} /* __cons_vbp_cls */


static void __cons_vbpiii_color(STATEPARAMS, int fore, int back, int bord)
{
} /* __cons_vbpiii_color */

#endif


#ifdef WIN32   /* stubs for now for win32. */
static int  __cons_openConsole(STATEPARAMS) { return(-1); }
static void __cons_deinitConsoleHandler(STATEPARAMS) {}
static void __cons_vbpS_print(STATEPARAMS, PBasicString pStr) {}
static void __cons_vbp_cls(STATEPARAMS) {}
static void __cons_vbpiii_color(STATEPARAMS, int fore, int back, int bord) {}
#endif

static void __cons_vbpii_viewPrint(STATEPARAMS, int top, int bottom)
{
    if ( (top < 0) || (bottom < top) || (bottomRow > (lines + 1)) )
        __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
    else
    {
        __setConsoleXY(0, top);
        winTop = __xyToConsoleMatrix(0, top);
        winBottom = __xyToConsoleMatrix(columns, bottom);
    } /* else */
} /* __cons_vbpii_viewPrint */


static void __cons_vbp_viewPrint(STATEPARAMS)
{
    __setConsoleXY(0, 0);
    winTop = __xyToConsoleMatrix(0, 0);
    winBottom = __xyToConsoleMatrix(columns, lines);
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


static int __cons_vbiA_pos(STATEPARAMS, void *pVar)
/*
 * Return current cursor column.
 *
 *    params : pVar == ignored, any expression.
 *   returns : current x-coordinate of cursor.
 */
{
    return(x);
} /* __cons_vbiA_pos */


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
        vbpS_print = __cons_vbpS_print;
        vbpii_viewPrint = __cons_vbpii_viewPrint;
        vbp_viewPrint = __cons_vbpii_viewPrint;
        vbp_cls = __cons_vbp_cls;
        vbi_csrline = __cons_vbi_csrline;
        vbiA_pos = __cons_vbiA_pos;
        vbpiii_color = __cons_vbpiii_color;
        vbpil_color = __cons_vbpil_color;
        vbpi_color = __cons_vbpi_color;
        retVal = true;
    } /* if */
} /* __initDirectConsole */

/* end of DirectConsole.c ... */

