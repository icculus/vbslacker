/*
 * Console support via stdout (for redirection) for BASIClib.
 *
 *    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <unistd.h>
#include "ConsoleFunctions.h"
#include "RedirectedConsole.h"


static void __redir_deinitConsole(STATEPARAMS)
{
    printf("\n");
} /* __redir_deinitConsole */


static void __redir_vbpV_print(STATEPARAMS, PVariant pVar)
{
    /* !!! write this! */
} /* __curs_vbpV_print */


static void __redir_vbpii_viewPrint(STATEPARAMS, int topRow, int bottomRow)
/*
 * This can't do anything, if we're redirecting, so just return.
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
 * This can't do anything, if we're redirecting, so just return.
 *
 *   params : void.
 *  returns : void.
 */
{
} /* __redir_vbp_viewPrint */



static void __redir_vbp_cls(STATEPARAMS)
/*
 * This can't do much, if we're redirecting, so just send on a newline,
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
 * Can't manipulate the cursor in redirecting mode, so return (0).
 *
 *    params : void.
 *   returns : always (0);
 */
{
    return(0);
} /* __redir_vbi_csrline */


static int __curs_vbiA_pos(STATEPARAMS, void *pVar)
/*
 * Can't manipulate the cursor in redirecting mode, so return (0).
 *
 *    params : pVar == ignored value.
 *   returns : always (0);
 */
{
    return(0);
} /* vbiA_pos */


void vbpiii_color(STATEPARAMS, int foreground, int background, int border)
/*
 * This can't do anything, if we're redirecting, so just return.
 *
 *   params : void.
 *  returns : void.
 */
{
} /* vbpiii_color */


boolean __initRedirectingConsole(STATEPARAMS)
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
        __forceRedirectingConsole(STATEARGS);
        retVal = true;
    } /* if */

    return(retVal);
} /* __initRedirectingConsole */


void __forceRedirectingConsole(STATEPARAMS)
/*
 * Force the function pointers to use the redirecting console to write
 *  to stdout, regardless of whether we're redirecting or not.
 *
 *     params : void.
 *    returns : void.
 */
{
    __deinitConsole = __redir_deinitConsole;
    __vbpS_print = __redir_vbpS_print;
    __vbpii_viewPrint = __redir_vbpii_viewPrint;
    __vbp_viewPrint = __redir_vbpii_viewPrint;
    __vbp_cls = __redir_vbp_cls;
    __vbi_csrline = __redir_vbi_csrline;
    __vbiA_pos = __redir_vbiA_pos;
    __vbpiii_color = __redir_vbpiii_color;
} /* __forceRedirectingConsole */

/* end of RedirectingConsole.c ... */

