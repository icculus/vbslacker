/*
 * Console support via stdout (for redirection) for BASIClib.
 *
 *    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <unistd.h>
#include "ConsoleFunctions.h"
#include "RedirectedConsole.h"


static void __redir_deinitConsoleHandler(STATEPARAMS)
{
    printf("\n");
} /* __redir_deinitConsole */


static void __redir_vbpS_print(STATEPARAMS, PBasicString pStr)
{
    /* !!! write this! */
} /* __redir_vbpS_print */


static void __redir_vbpii_viewPrint(STATEPARAMS, int topRow, int bottomRow)
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


static void __redir_vbp_viewPrint(STATEPARAMS)
/*
 * This can't do anything, if we're redirected, so just return.
 *
 *   params : void.
 *  returns : void.
 */
{
} /* __redir_vbp_viewPrint */



static void __redir_vbp_cls(STATEPARAMS)
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


static int __redir_vbi_csrline(STATEPARAMS)
/*
 * Can't manipulate the cursor in redirected mode, so return (0).
 *
 *    params : void.
 *   returns : always (0);
 */
{
    return(0);
} /* __redir_vbi_csrline */


static int __redir_vbiA_pos(STATEPARAMS, void *pVar)
/*
 * Can't manipulate the cursor in redirected mode, so return (0).
 *
 *    params : pVar == ignored value.
 *   returns : always (0);
 */
{
    return(0);
} /* vbiA_pos */


static void __redir_vbpiii_color(STATEPARAMS, int fore, int back, int bord)
/*
 * This can't do anything, if we're redirected, so just return.
 *
 *   params : void.
 *  returns : void.
 */
{
} /* __redir_vbpiii_color */


static void __redir_vbpil_color(STATEPARAMS, int fore, long feh)
/*
 * This is for graphic modes only; throw an error.
 *
 *   params : fore == would-be foreground color.
 *            feh == palette control, or some crap.
 *  returns : void.
 */
{
    __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
} /* __redir_vbpil_color */


static void __redir_vbpi_color(STATEPARAMS, int fore)
/*
 * This is for graphic modes only; throw an error.
 *
 *   params : fore == would-be foreground color.
 *  returns : void.
 */
{
    __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
} /* __redir_vbpi_color */


static void __redir_getConsoleHandlerName(STATEPARAMS, char *buffer, int size)
{
    strncpy(buffer, "RedirectedConsole", size);
} /* __redir_getConsoleHandlerName */


boolean __initRedirectedConsole(STATEPARAMS)
/*
 * Attempt to initialize curses library access.
 *
 *   params : void.
 *  returns : boolean true if initialized, false on error.
 */
{
    boolean retVal = false;

    if (isatty(STDIN_FILENO) == 0)
    {
        __forceRedirectedConsole(STATEARGS);
        retVal = true;
    } /* if */

    return(retVal);
} /* __initRedirectedConsole */


void __forceRedirectedConsole(STATEPARAMS)
/*
 * Force the function pointers to use the redirected console to write
 *  to stdout, regardless of whether we're redirected or not.
 *
 *     params : void.
 *    returns : void.
 */
{
    __getConsoleHandlerName = __redir_getConsoleHandlerName;
    __deinitConsoleHandler = __redir_deinitConsoleHandler;
    vbpS_print = __redir_vbpS_print;
    vbpii_viewPrint = __redir_vbpii_viewPrint;
    vbp_viewPrint = __redir_vbpii_viewPrint;
    vbp_cls = __redir_vbp_cls;
    vbi_csrline = __redir_vbi_csrline;
    vbiA_pos = __redir_vbiA_pos;
    vbpiii_color = __redir_vbpiii_color;
    vbpil_color = __redir_vbpil_color;
    vbpi_color = __redir_vbpi_color;
} /* __forceRedirectedConsole */

/* end of RedirectedConsole.c ... */
