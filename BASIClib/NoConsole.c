/*
 * Stub functions for when console support is disabled. BASIClib
 *  throws runtime errors in this case.
 *
 *    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "ConsoleFunctions.h"
#include "NoConsole.h"

static void __nocons_deinitConsoleHandler(STATEPARAMS) {}

static void __nocons_vbpS_print(STATEPARAMS, PBasicString x)
{
    __runtimeError(STATEARGS, ERR_CANNOT_CONTINUE);
} /* __nocons_vbpS_print */

static void __nocons_vbpii_viewPrint(STATEPARAMS, int top, int bottom)
{
    __runtimeError(STATEARGS, ERR_CANNOT_CONTINUE);
} /* __nocons_vbpii_viewPrint */

static void __nocons_vbp_viewPrint(STATEPARAMS)
{
    __runtimeError(STATEARGS, ERR_CANNOT_CONTINUE);
} /* __nocons_vbp_viewPrint */

static void __nocons_vbp_cls(STATEPARAMS)
{
    __runtimeError(STATEARGS, ERR_CANNOT_CONTINUE);
} /* __nocons_vbp_cls */

static int __nocons_vbi_csrline(STATEPARAMS)
{
    __runtimeError(STATEARGS, ERR_CANNOT_CONTINUE);
} /* __nocons_vbi_csrline */

static int  __nocons_vbiA_pos(STATEPARAMS, void *pVar)
{
    __runtimeError(STATEARGS, ERR_CANNOT_CONTINUE);
} /* __nocons_vbiA_pos */

static void __nocons_vbpiii_color(STATEPARAMS, int fore, int back, int bord)
{
    __runtimeError(STATEARGS, ERR_CANNOT_CONTINUE);
} /* __nocons_vbpiii_color */

static void __nocons_vbpil_color(STATEPARAMS, int fore, long feh)
{
    __runtimeError(STATEARGS, ERR_CANNOT_CONTINUE);
} /* __nocons_vbpil_color */

static void __nocons_vbpi_color(STATEPARAMS, int fore)
{
    __runtimeError(STATEARGS, ERR_CANNOT_CONTINUE);
} /* __nocons_vbpi_color */


static void __nocons_getConsoleHandlerName(STATEPARAMS, char *buffer, int size)
{
    strncpy(buffer, "NoConsole", size);
} /* __nocons_getConsoleHandlerName */


boolean __initNoConsole(STATEPARAMS)
/*
 * Check if console is disabled, and if so, set all console functions to
 *  error-throwing stubs.
 *
 *   params : void.
 *  returns : Returns (true) if set as new console handler, (false) otherwise.
 */
{
    boolean retVal;

    if (__getInitFlags(STATEARGS) & INITFLAG_DISABLE_CONSOLE)
    {
        __getConsoleHandlerName = __nocons_getConsoleHandlerName;
        __deinitConsoleHandler = __nocons_deinitConsoleHandler;
        vbpS_print = __nocons_vbpS_print;
        vbpii_ViewPrint = __nocons_vbpii_ViewPrint;
        vbp_ViewPrint = __nocons_vbpii_ViewPrint;
        vbp_cls = __nocons_vbp_cls;
        vbi_csrline = __nocons_vbi_csrline;
        vbiA_pos = __nocons_vbiA_pos;
        vbpiii_color = __nocons_vbpiii_color;
        vbpil_color = __nocons_vbpil_color;
        vbpi_color = __nocons_vbpi_color;
        retVal = true;
    } /* if */

    return(retVal);
} /* __initNoConsole */

/* end of NoConsole.c ... */

