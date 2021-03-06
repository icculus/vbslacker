/*
 * Direct Linux console support for BASIClib.
 *
 *    Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <termios.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "StdBasic.h"

static void __dcon_deinitConsoleDriver(void);
static void __dcon_printNChars(const __byte *str, __long n);
static void __dcon_printNewLine(void);
static void __dcon_vbpii_viewPrint(__integer topRow, __integer botmRow);
static void __dcon_vbp_viewPrint(void);
static void __dcon_vbp_cls(void);
static __integer __dcon_vbi_csrline(void);
static __integer __dcon_vbia_pos(void *pVar);
static void __dcon_vbpiii_color(__integer fore,
                                 __integer back,
                                 __integer bord);
static void __dcon_vbpil_color(__integer fore, __long feh);
static void __dcon_vbpi_color(__integer fore);
static void __dcon_vbpii_locate(__integer newY, __integer newX);
static void __dcon_vbpNi_locate(__integer newX);
static void __dcon_vbpiN_locate(__integer newY);
static void __dcon_vbp_locate(void);
static void __dcon_vbpif_sound(__integer frequency, __single duration);
static void __dcon_vbp_beep(void);
static void __dcon_getConsoleDriverName(__byte *buffer, __integer size);

static __vbConsoleEntries directConEntries =
{
    __dcon_getConsoleDriverName,
    __dcon_deinitConsoleDriver,
    __dcon_printNewLine,
    __dcon_printNChars,
    __dcon_vbpii_viewPrint,
    __dcon_vbp_viewPrint,
    __dcon_vbp_cls,
    __dcon_vbi_csrline,
    __dcon_vbia_pos,
    __dcon_vbpiii_color,
    __dcon_vbpil_color,
    __dcon_vbpi_color,
    __dcon_vbpii_locate,
    __dcon_vbpNi_locate,
    __dcon_vbpiN_locate,
    __dcon_vbp_locate,
    __dcon_vbpif_sound,
    __dcon_vbp_beep
};

#ifdef LINUX   /* Completely non-portable Linux-specific support. */
#   include <signal.h>
#   include <sys/ioctl.h>
#   include <sys/vt.h>
#   include <sys/kd.h>
#   include <sys/param.h>
#endif


#define ASCII_BEEP 7
#define ASCII_CR   13
#define ASCII_NL   10
#define ASCII_TAB  9

#define __xyToConsoleMatrix(x, y)  (((y) * ((columns + 1) * 2)) + ((x) * 2) + 4)
#define __consoleMatrixToX(pos)    (((pos - 4) / 2) % (columns + 1))
#define __consoleMatrixToY(pos)    (((pos - 4) / 2) / (columns + 1))

    /* various console attributes... */
static __integer x = 0;
static __integer y = 0;
static __integer lines = 0;
static __integer columns = 0;
static __integer winTop = 0;
static __integer winBottom = 0;

static ThreadLock consoleLock;



#ifdef LINUX   /* Completely non-portable Linux-specific support. */

#define DEVNAME_VCSA "/dev/vcsa"

static int cons = -1;        /* file handle of /dev/vcsa? device... */
static int ttyHandle = -1;   /* file handle of /dev/tty for beeps.  */
static unsigned char curColor = 7;
static struct termios termStuff;
static tcflag_t termAttr;

static void __setCursorXY(__integer _x, __integer _y)
/*
 * update our cursor position variables, and /dev/vcsa?'s records.
 *
 *      params : _x, _y == new cursor position.
 *     returns : void.
 */
{
    unsigned char buf[2] = {(unsigned char) _x, (unsigned char) _y};

    __obtainThreadLock(&consoleLock);
    x = _x;
    y = _y;
    lseek(cons, 2, SEEK_SET);        /* position in /dev/vcsa? of X cursor. */
    write(cons, buf, sizeof (buf));  /* update the cursor. */
    __releaseThreadLock(&consoleLock);
} /* __setCursorXY */


static void __scrollConsole(void)
/*
 * Move all but the top line of the printable window up one line, and
 *  blank the bottom line.
 *
 *    params : void.
 *   returns : void.
 */
{
    __integer columnBytes = (columns + 1) * 2;
    __integer scrollStart = winTop + columnBytes;
    __integer bufSize = (winBottom - scrollStart) + 1;
    __byte scrollBuf[bufSize];
    __byte blankBuf[columnBytes];
    __integer i;

    for (i = 0; i < columnBytes; i += 2)
    {
        blankBuf[i]     = ' ';
        blankBuf[i + 1] = curColor;
    } /* for */

    __obtainThreadLock(&consoleLock);
    lseek(cons, scrollStart, SEEK_SET); /* read the console. */
    read(cons, scrollBuf, bufSize);

    lseek(cons, winTop, SEEK_SET);  /* copy it back, one line higher... */
    write(cons, scrollBuf, bufSize);
    lseek(cons, (winBottom - columnBytes) + 2, SEEK_SET);
    write(cons, blankBuf, columnBytes);  /* fill last line with spaces... */
    __releaseThreadLock(&consoleLock);
} /* __scrollConsole */


static __boolean __setupInput(__attribute__ ((unused)) int vtNum)
{
    if (tcgetattr(STDOUT_FILENO, &termStuff) == -1)
        return(false);

    termAttr = termStuff.c_lflag;
    termStuff.c_lflag &= !ECHO;
    termStuff.c_lflag &= !ECHONL;

    if (tcsetattr(STDOUT_FILENO, TCSANOW, &termStuff) == -1)
        return(false);

    return(true);
} /* __setupInput */


static __boolean __setupOutput(int vtNum)
{
    char consDev[strlen(DEVNAME_VCSA) + 20];
    unsigned char br[4];

    sprintf(consDev, "%s%d", DEVNAME_VCSA, vtNum);

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
            __createThreadLock(&consoleLock);
            lines   = ( ((__integer) br[0]) - 1 );
            columns = ( ((__integer) br[1]) - 1 );
            x = ( ((__integer) br[2]) - 1 );
            y = ( ((__integer) br[3]) - 1 );
            curColor = 7;
            winTop = __xyToConsoleMatrix(0, 0);
            winBottom = __xyToConsoleMatrix(columns, lines);
        } /* else */
    } /* if */

    return((cons == -1) ? false : true);
} /* __setupOutput */


static int getVirtualConsoleHandle(void)
{
    struct vt_mode vtMode;

    ttyHandle = open("/dev/tty", O_RDWR);
    if (ttyHandle > 0)
    {
            /* this ioctl() verifies that this is really a VC... */
        if (ioctl(ttyHandle, VT_GETMODE, &vtMode) < 0)
        {
            close(ttyHandle);
            ttyHandle = -1;
        } /* if */
    } /* if */

    return(ttyHandle);
} /* getVirtualConsoleHandle */


static int getVirtualConsoleNumber(int ttyFileHandle)
{
    struct vt_stat vtState;

    ioctl(ttyFileHandle, VT_GETSTATE, &vtState);
    return(vtState.v_active);
} /* getVirtualConsoleNumber */


static __boolean __setupIO(int vtNum)
{
    __boolean retVal = false;

    if ((__setupInput(vtNum) == true) && (__setupOutput(vtNum) == true))
        retVal = true;

    return(retVal);
} /* __setupIO */


static __boolean __dcon_openConsole(void)
/*
 * Open the virtual console for read/write, if at a virtual console, and
 *  the kernel will permit it.
 *
 *    params : void.
 *   returns : false on error, true on success.
 */
{
    __boolean retVal = false;
    int vtNum;

    if (getVirtualConsoleHandle() > 0)
    {
        vtNum = getVirtualConsoleNumber(ttyHandle);
        retVal = __setupIO(vtNum);
        if (retVal == false)
        {
            if (ttyHandle != -1)
                close(ttyHandle);

            if (cons != -1)
                close(cons);

            cons = ttyHandle = -1;
        } /* if */
    } /* if */

    return(retVal);
} /* __dcon_openConsole */


static void __dcon_deinitConsoleDriver(void)
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
        __obtainThreadLock(&consoleLock);
        if (y >= lines)
        {
                /*
                 * Console won't scroll correctly unless we
                 *  default winTop and winBottom...
                 */
            winTop = __xyToConsoleMatrix(0, 0);
            winBottom = __xyToConsoleMatrix(columns, lines);
            __scrollConsole();
            y = lines - 1;
        } /* if */
        __setCursorXY(0, y + 1);
        close(cons);
        cons = -1;
#warning comment.
        termStuff.c_lflag = termAttr;
        tcsetattr(STDOUT_FILENO, TCSANOW, &termStuff);
        __releaseThreadLock(&consoleLock);
        __destroyThreadLock(&consoleLock);
    } /* if */

    if (ttyHandle != -1)
    {
        close(ttyHandle);
        ttyHandle = -1;
    } /* if */
} /* __dcon_deinitConsole */


static void __dcon_vbpif_sound(__integer frequency, __single duration)
{
        /*
         * Original BASIC specs want (duration) in Intel clock ticks.
         *  There are 18.2 clock ticks per second on Intel chips.
         */
    double seconds = ((double) duration) / ((double) 18.2);
    int hundredths = ((int) (seconds * 100.0));
    unsigned int ticks = hundredths * HZ / 100;

        /* Verify legal values for kernel... */
    if (ticks > 0xffff)
        ticks = 0xffff;

        /* check validity of ticks, too... */
    if ((hundredths > 0) && (ticks == 0))
        ticks = 1;

    if (ticks > 0)
        ioctl(ttyHandle, KDMKTONE, (((1193180 / frequency) << 16) | ticks));
} /* __dcon_vbpif_sound */


static void __dcon_vbp_beep(void)
{
    __dcon_vbpif_sound(750, 5.0 * 18.2); /* !!! test. */
} /* __dcon_vbp_beep */


static void __dcon_printNChars(const __byte *str, __long n)
/*
 * Write (n) characters at pos (str) to the printable window,
 *  scrolling if needed, and moving the cursor to the new position.
 *
 *   params : str == characters to write.
 *            n == char count to write.
 *  returns : void.
 */
{
    __long i;
    __byte matrixChar[2];

    __obtainThreadLock(&consoleLock);

    matrixChar[1] = curColor;   /* attribute byte. */

    for (i = 0; i < n; i++)
    {
        lseek(cons, __xyToConsoleMatrix(x, y), SEEK_SET);
        switch (str[i])
        {
            case ASCII_BEEP:
                __dcon_vbp_beep();
                break;

            case ASCII_CR:
                if ((i + 1 < n) && (str[i] == ASCII_NL))
                    i++;
                __printNewLine();
                break;

            case ASCII_NL:
                __printNewLine();
                break;

            case ASCII_TAB:
                matrixChar[0] = ' ';
                /* check end of line...  !!! */
                do
                {
                    if (x <= columns)
                    {    
                        write(cons, matrixChar, sizeof (matrixChar));
                        x++;
                    } /* if */
                } while (x % 8);
                break;

            default:  /* "normal" chars ... */
                matrixChar[0] = str[i];
                write(cons, matrixChar, sizeof (matrixChar));
                x++;
                break;
        } /* switch */

        if (x > columns)
        {
            y++;
            x = 0;
        } /* if */

        if (__xyToConsoleMatrix(x, y) > winBottom)
        {
            __scrollConsole();
            y = __consoleMatrixToY(winBottom);
        } /* if */
    } /* for */

    __setCursorXY(x, y);
    __releaseThreadLock(&consoleLock);
} /* cons_printNChars */


static void __dcon_printNewLine(void)
/*
 * Move the cursor down to the start of the next line. Scroll if necessary.
 *
 *    params : void.
 *   returns : void.
 */
{
    __obtainThreadLock(&consoleLock);

    x = 0;
    y++;

    if (__xyToConsoleMatrix(x, y) > winBottom)
    {
        __scrollConsole();
        y = __consoleMatrixToY(winBottom);
    } /* if */
    __setCursorXY(x, y);

    __releaseThreadLock(&consoleLock);
} /* __dcon_printNewLine */


static void __dcon_vbp_cls(void)
/*
 * Clear the current printable window. The window will be blanked of
 *  characters, and set to the background color. The cursor is moved to
 *  the upper-left hand corner of the printable window.
 *
 *     params : void.
 *    returns : void.
 */
{
    __integer max = (winBottom + 1) - winTop;
    __byte buffer[max];
    __integer i;

    for (i = 0; i < max; i += 2)
    {
        buffer[i] = ' ';
        buffer[i + 1]     = curColor;
    } /* for */

    __obtainThreadLock(&consoleLock);
    lseek(cons, winTop, SEEK_SET);
    write(cons, buffer, max);
    __setCursorXY(__consoleMatrixToX(winTop),
                   __consoleMatrixToY(winTop));
    __releaseThreadLock(&consoleLock);
} /* __dcon_vbp_cls */


static void __dcon_vbpiii_color(__integer fore, __integer back,
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
    #warning Do something with this.
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
//    __dcon_vbpii_color(fore, back);
} /* __dcon_vbpiii_color */

#endif


static void __dcon_vbpii_viewPrint(__integer top, __integer bottom)
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
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else
    {
        __obtainThreadLock(&consoleLock);
        winTop = __xyToConsoleMatrix(0, top);
        winBottom = __xyToConsoleMatrix(columns, bottom);
        __setCursorXY(0, top);
        __releaseThreadLock(&consoleLock);
    } /* else */
} /* __dcon_vbpii_viewPrint */


static void __dcon_vbp_viewPrint(void)
/*
 * Set the whole console window printable.
 *
 *     params : void.
 *    returns : void.
 */
{
    __obtainThreadLock(&consoleLock);
    winTop = __xyToConsoleMatrix(0, 0);
    winBottom = __xyToConsoleMatrix(columns, lines);
    __setCursorXY(0, 0);
    __releaseThreadLock(&consoleLock);
} /* __dcon_vbp_viewPrint */


static __integer __dcon_vbi_csrline(void)
/*
 * Return current cursor row.
 *
 *    params : void.
 *   returns : current y-coordinate of cursor.
 */
{
    return(y);
} /* __dcon_vbi_csrline */


static  __integer __dcon_vbia_pos(__attribute__ ((unused)) void *pVar)
/*
 * Return current cursor column.
 *
 *    params : pVar == ignored, any expression.
 *   returns : current x-coordinate of cursor.
 */
{
    return(x);
} /* __dcon_vbia_pos */


static void __dcon_vbpil_color(__attribute__ ((unused)) __integer fore,
                               __attribute__ ((unused)) __long feh)
/*
 * This form of the COLOR command is only for graphics mode, so throw a
 *  runtime error.
 */
{
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
} /* __dcon_vbpil_color */


static void __dcon_vbpi_color(__integer fore)
/*
 * This form of the COLOR command is only for graphics mode, so throw a
 *  runtime error.
 */
{
    if ((fore < 0) || (fore > 15))
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else
        curColor = fore;  /* !!! */
} /* __dcon_vbpi_color */


static void __dcon_vbpii_locate(__integer newY, __integer newX)
{
    newY--;
    newX--;
/* !!! wintop, wwinbottom! */
    if ((newX > columns) || (newY > lines) || (newX < 0) || (newY < 0))
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else
        __setCursorXY(newX, newY);
} /* __dcon_vbpii_locate */


static void __dcon_vbpNi_locate(__integer newX)
{
    newX--;

    if ((newX < 0) || (newX > columns))
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else
        __setCursorXY(newX, y);
} /* __dcon_vbpNi_locate */


static void __dcon_vbpiN_locate(__integer newY)
{
    newY--;

    if ((newY < 0) || (newY > lines))
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else
        __setCursorXY(x, newY);
} /* __dcon_vbpiN_locate */


static void __dcon_vbp_locate(void)
{
    /* do nothing. */
} /* __dcon_vbp_locate */


static void __dcon_getConsoleDriverName(__byte *buffer, __integer size)
/*
 * (Getting rather object-oriented...) copy the name of this console
 *  handler to a buffer.
 *
 *      params : buffer == allocated buffer to copy name to.
 *               size   == maximum bytes to copy to buffer.
 *     returns : void.
 */
{
    strncpy(buffer, "DirectLinuxConsole", size);
} /* __dcon_getConsoleDriverName */


__boolean __initDirectConsole(__vbConsoleEntries *entries)
/*
 * Attempt to initialize direct-to-console access.
 *
 *   params : void.
 *  returns : boolean true if initialized, false on error.
 */
{
    __boolean retVal = __dcon_openConsole();

    if (retVal == true)
        memcpy(entries, &directConEntries, sizeof (__vbConsoleEntries));

    return(retVal);
} /* __initDirectConsole */

/* end of DirectConsole.c ... */

