/*
 * Console support via stdout (for redirection) for BASIClib.
 *
 *    Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "ConsoleFunctions.h"
#include "RedirectedConsole.h"


static void __redir_deinitConsoleDriver(void)
/*
 * Deinitialize this console driver. In this case, just bump the
 *  cursor down to the start of the next line...
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n");
} /* __redir_deinitConsole */


static void __redir_printNChars(__byte *str, __long n)
/*
 * Write (n) chars from (str) to the printable window, scrolling if 
 *  needed, and moving the cursor to the new position.
 *
    params : str == chars to write.
 *  returns : void.
 */
{
    __long i;

    for (i = 0; i < n; i++)
        putchar(str[i]);
} /* __redir_vbpS_print */


static void __redir_printNewLine(void)
/*
 * Move the cursor down to the start of the next line. Scroll if necessary.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n");
} /* __redir_printNewLine */


static void __redir_vbpii_viewPrint(__integer topRow, __integer bottomRow)
/*
 * This can't do anything, if we're redirected, so just return.
 *
 *   params : topRow == what would be new top of window.
 *            bottomRow == what would be new bottom of window.
 *  returns : void.
 */
{
    /* do nothing. */
} /* __redir_vbpii_viewPrint */


static void __redir_vbp_viewPrint(void)
/*
 * This can't do anything, if we're redirected, so just return.
 *
 *   params : void.
 *  returns : void.
 */
{
    /* do nothing. */
} /* __redir_vbp_viewPrint */



static void __redir_vbp_cls(void)
/*
 * This can't do much, if we're redirected, so just send on a newline,
 *  and return.
 *
 *   params : void.
 *  returns : void.
 */
{
    printf("\n");
} /* __redir_vbp_cls */


static __integer __redir_vbi_csrline(void)
/*
 * Can't manipulate the cursor in redirected mode, so return (0).
 *
 *    params : void.
 *   returns : always (0);
 */
{
    return(0);
} /* __redir_vbi_csrline */


static __integer __redir_vbia_pos(void *pVar)
/*
 * Can't manipulate the cursor in redirected mode, so return (0).
 *
 *    params : pVar == ignored value.
 *   returns : always (0);
 */
{
    return(0);
} /* vbia_pos */


static void __redir_vbpiii_color(__integer fore, __integer back, __integer bord)
/*
 * This can't do anything, if we're redirected, so just return.
 *
 *   params : void.
 *  returns : void.
 */
{
    /* do nothing. */
} /* __redir_vbpiii_color */


static void __redir_vbpil_color(__integer fore, __long feh)
/*
 * This is for graphic modes only; throw an error.
 *
 *   params : fore == would-be foreground color.
 *            feh == palette control, or some crap.
 *  returns : void.
 */
{
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
} /* __redir_vbpil_color */


static void __redir_vbpi_color(__integer fore)
/*
 * This is for graphic modes only; throw an error.
 *
 *   params : fore == would-be foreground color.
 *  returns : void.
 */
{
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
} /* __redir_vbpi_color */


static void __redir_vbpii_locate(__integer newY, __integer newX)
{
} /* __redir_vbpii_locate */


static void __redir_vbpNi_locate(__integer newX)
{
} /* __redir_vbpNi_locate */


static void __redir_vbpiN_locate(__integer newY)
{
} /* __redir_vbpiN_locate */


static void __redir_vbp_locate(void)
{
} /* __redir_vbp_locate */


static void __redir_playSound(__integer frequency, __single duration)
{
} /* __redir_playSound */


static void __redir_vbp_beep(void)
{
} /* __redir_vbp_beep */


static void __redir_getConsoleDriverName(__byte *buffer, __integer size)
/*
 * (Getting rather object-oriented...) copy the name of this console
 *  driver to a buffer.
 *
 *      params : buffer == allocated buffer to copy name to.
 *               size   == maximum bytes to copy to buffer.
 *     returns : void.
 */
{
    strncpy(buffer, "RedirectedConsole", size);
} /* __redir_getConsoleDriverName */


__boolean __initRedirectedConsole(void)
/*
 * Attempt to initialize curses library access.
 *
 *   params : void.
 *  returns : boolean true if initialized, false on error.
 */
{
    __boolean retVal = false;

    if ((isatty(STDIN_FILENO) == 0) || (isatty(STDOUT_FILENO) == 0))
    {
        __forceRedirectedConsole();
        retVal = true;
    } /* if */

    return(retVal);
} /* __initRedirectedConsole */


void __forceRedirectedConsole(void)
/*
 * Force the function pointers to use the redirected console to write
 *  to stdout, regardless of whether we're redirected or not.
 *
 *     params : void.
 *    returns : void.
 */
{
    __getConsoleDriverName = __redir_getConsoleDriverName;
    __deinitConsoleDriver = __redir_deinitConsoleDriver;
    __printNewLine = __redir_printNewLine;
    __printNChars = __redir_printNChars;
    __playSound = __redir_playSound;
    _vbpii_viewPrint = __redir_vbpii_viewPrint;
    _vbp_viewPrint = __redir_vbp_viewPrint;
    _vbp_cls = __redir_vbp_cls;
    _vbi_csrline = __redir_vbi_csrline;
    _vbia_pos = __redir_vbia_pos;
    _vbpiii_color = __redir_vbpiii_color;
    _vbpil_color = __redir_vbpil_color;
    _vbpi_color = __redir_vbpi_color;
    _vbpii_locate = __redir_vbpii_locate;
    _vbpNi_locate = __redir_vbpNi_locate;
    _vbpiN_locate = __redir_vbpiN_locate;
    _vbp_locate = __redir_vbp_locate;
    _vbp_beep = __redir_vbp_beep;
} /* __forceRedirectedConsole */

/* end of RedirectedConsole.c ... */

