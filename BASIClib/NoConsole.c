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

static void __nocons_printNChars(__byte *str, __long n)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpS_print */

static void __nocons_printNewLine(void)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_printNewLine */

static void __nocons_vbpii_viewPrint(__integer top, __integer bottom)
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

static __integer __nocons_vbi_csrline(void)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
    return(0);  /* never hits this. */
} /* __nocons_vbi_csrline */

static __integer __nocons_vbia_pos(void *pVar)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
    return(0);  /* never hits this. */
} /* __nocons_vbia_pos */

static void __nocons_vbpiii_color(__integer fore, __integer back, __integer bord)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpiii_color */

static void __nocons_vbpil_color(__integer fore, __long feh)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpil_color */

static void __nocons_vbpi_color(__integer fore)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpi_color */

static void __nocons_getConsoleHandlerName(__byte *buffer, __integer size)
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
    __boolean retVal = ( (__getInitFlags() & INITFLAG_DISABLE_CONSOLE) ?
                         true : false );

    if (retVal == true)
    {
        __getConsoleHandlerName = __nocons_getConsoleHandlerName;
        __deinitConsoleHandler = __nocons_deinitConsoleHandler;
        __printNewLine = __nocons_printNewLine;
        __printNChars = __nocons_printNChars;
        _vbpii_viewPrint = __nocons_vbpii_viewPrint;
        _vbp_viewPrint = __nocons_vbp_viewPrint;
        _vbp_cls = __nocons_vbp_cls;
        _vbi_csrline = __nocons_vbi_csrline;
        _vbia_pos = __nocons_vbia_pos;
        _vbpiii_color = __nocons_vbpiii_color;
        _vbpil_color = __nocons_vbpil_color;
        _vbpi_color = __nocons_vbpi_color;
    } /* if */

    return(retVal);
} /* __initNoConsole */

/* end of NoConsole.c ... */

