/*
 * Stub functions for when console support is disabled. BASIClib
 *  throws runtime errors in this case.
 *
 *    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "ConsoleFunctions.h"
#include "NoConsole.h"

static void __nocons_deinitConsole(STATEPARAMS) {}

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
        __deinitConsole = __nocons_deinitConsole;
        __vbpS_print = __nocons_vbpS_print;
        __vbpii_ViewPrint = __nocons_vbpii_ViewPrint;
        __vbp_ViewPrint = __nocons_vbpii_ViewPrint;
        __vbp_cls = __nocons_vbp_cls;
        __vbi_csrline = __nocons_vbi_csrline;
        __vbiA_pos = __nocons_vbiA_pos;
        __vbpiii_color = __nocons_vbpiii_color;
        retVal = true;
    } /* if */

    return(retVal);
} /* __initNoConsole */

/* end of NoConsole.c ... */

