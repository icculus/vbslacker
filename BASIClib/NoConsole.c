/*
 * Stub functions for when console support is disabled. BASIClib
 *  throws runtime errors in this case.
 *
 *    Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <string.h>
#include "ConsoleFunctions.h"
#include "NoConsole.h"

static void __nocons_deinitConsoleHandler(void) {}

static void __nocons_printNChars(char *str, int n)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpS_print */

static void __nocons_printNewLine(void)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_printNewLine */

static void __nocons_vbpii_viewPrint(int top, int bottom)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpii_viewPrint */

static void __nocons_vbp_viewPrint(void)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbp_viewPrint */

static void __nocons_vbp_cls(void)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbp_cls */

static int __nocons_vbi_csrline(void)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
    return(0);  /* never hits this. */
} /* __nocons_vbi_csrline */

static int  __nocons_vbia_pos(void *pVar)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
    return(0);  /* never hits this. */
} /* __nocons_vbia_pos */

static void __nocons_vbpiii_color(int fore, int back, int bord)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpiii_color */

static void __nocons_vbpil_color(int fore, long feh)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpil_color */

static void __nocons_vbpi_color(int fore)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpi_color */

static void __nocons_getConsoleHandlerName(char *buffer, int size)
/*
 * (Getting rather object-oriented...) copy the name of this console
 *  handler to a buffer.
 *
 *      params : buffer == allocated buffer to copy name to.
 *               size   == maximum bytes to copy to buffer.
 *     returns : void.
 */
{
    strncpy(buffer, "NoConsole", size);
} /* __nocons_getConsoleHandlerName */

__boolean __initNoConsole(void)
/*
 * Check if console is disabled, and if so, set all console functions to
 *  error-throwing stubs.
 *
 *   params : void.
 *  returns : Returns (true) if set as new console handler, (false) otherwise.
 */
{
    __boolean retVal;

    if (__getInitFlags() & INITFLAG_DISABLE_CONSOLE)
    {
        __getConsoleHandlerName = __nocons_getConsoleHandlerName;
        __deinitConsoleHandler = __nocons_deinitConsoleHandler;
        __printNewLine = __nocons_printNewLine;
        __printNChars = __nocons_printNChars;
        vbpii_viewPrint = __nocons_vbpii_viewPrint;
        vbp_viewPrint = __nocons_vbp_viewPrint;
        vbp_cls = __nocons_vbp_cls;
        vbi_csrline = __nocons_vbi_csrline;
        vbia_pos = __nocons_vbia_pos;
        vbpiii_color = __nocons_vbpiii_color;
        vbpil_color = __nocons_vbpil_color;
        vbpi_color = __nocons_vbpi_color;
        retVal = true;
    } /* if */

    return(retVal);
} /* __initNoConsole */

/* end of NoConsole.c ... */

