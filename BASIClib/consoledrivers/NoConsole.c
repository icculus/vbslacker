/*
 * Stub functions for when console support is disabled. BASIClib
 *  throws runtime errors in this case.
 *
 *    Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <string.h>
#include "StdBasic.h"

#warning these CANNOT_CONTINUEs should probably be ILLEGAL_FUNCTION_CALLs

static void __nocons_deinitConsoleDriver(void);
static void __nocons_printNChars(const __byte *str, __long n);
static void __nocons_printNewLine(void);
static void __nocons_vbpii_viewPrint(__integer topRow, __integer botmRow);
static void __nocons_vbp_viewPrint(void);
static void __nocons_vbp_cls(void);
static __integer __nocons_vbi_csrline(void);
static __integer __nocons_vbia_pos(void *pVar);
static void __nocons_vbpiii_color(__integer fore,
                                 __integer back,
                                 __integer bord);
static void __nocons_vbpil_color(__integer fore, __long feh);
static void __nocons_vbpi_color(__integer fore);
static void __nocons_vbpii_locate(__integer newY, __integer newX);
static void __nocons_vbpNi_locate(__integer newX);
static void __nocons_vbpiN_locate(__integer newY);
static void __nocons_vbp_locate(void);
static void __nocons_vbpif_sound(__integer frequency, __single duration);
static void __nocons_vbp_beep(void);
static void __nocons_getConsoleDriverName(__byte *buffer, __integer size);

static __vbConsoleEntries noConEntries =
{
    __nocons_getConsoleDriverName,
    __nocons_deinitConsoleDriver,
    __nocons_printNewLine,
    __nocons_printNChars,
    __nocons_vbpii_viewPrint,
    __nocons_vbp_viewPrint,
    __nocons_vbp_cls,
    __nocons_vbi_csrline,
    __nocons_vbia_pos,
    __nocons_vbpiii_color,
    __nocons_vbpil_color,
    __nocons_vbpi_color,
    __nocons_vbpii_locate,
    __nocons_vbpNi_locate,
    __nocons_vbpiN_locate,
    __nocons_vbp_locate,
    __nocons_vbpif_sound,
    __nocons_vbp_beep
};



static void __nocons_deinitConsoleDriver(void) {}

static void __nocons_printNChars(__attribute__ ((unused)) const __byte *str,
                                 __attribute__ ((unused)) __long n)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpS_print */

static void __nocons_printNewLine(void)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_printNewLine */

static void __nocons_vbpii_viewPrint(__attribute__ ((unused)) __integer top,
                                     __attribute__ ((unused)) __integer bottom)
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

static __integer __nocons_vbia_pos(__attribute__ ((unused)) void *pVar)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
    return(0);  /* never hits this. */
} /* __nocons_vbia_pos */

static void __nocons_vbpiii_color(__attribute__ ((unused)) __integer fore,
                                  __attribute__ ((unused)) __integer back,
                                  __attribute__ ((unused)) __integer bord)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpiii_color */

static void __nocons_vbpil_color(__attribute__ ((unused)) __integer fore,
                                 __attribute__ ((unused)) __long feh)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpil_color */

static void __nocons_vbpi_color(__attribute__ ((unused)) __integer fore)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpi_color */

static void __nocons_vbpii_locate(__attribute__ ((unused)) __integer newY,
                                  __attribute__ ((unused)) __integer newX)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpii_locate */

static void __nocons_vbpNi_locate(__attribute__ ((unused)) __integer newX)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpNi_locate */

static void __nocons_vbpiN_locate(__attribute__ ((unused)) __integer newY)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpiN_locate */

static void __nocons_vbp_locate(void)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbp_locate */

static void __nocons_vbpif_sound(__attribute__ ((unused)) __integer frequency,
                               __attribute__ ((unused)) __single duration)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbpif_sound */

static void __nocons_vbp_beep(void)
{
    __runtimeError(ERR_CANNOT_CONTINUE);
} /* __nocons_vbp_beep */

static void __nocons_getConsoleDriverName(__byte *buffer, __integer size)
/*
 * (Getting rather object-oriented...) copy the name of this console
 *  driver to a buffer.
 *
 *      params : buffer == allocated buffer to copy name to.
 *               size   == maximum bytes to copy to buffer.
 *     returns : void.
 */
{
    strncpy(buffer, "NoConsole", size);
} /* __nocons_getConsoleDriverName */


__boolean __initNoConsole(__vbConsoleEntries *entries)
/*
 * Check if console is disabled, and if so, set all console functions to
 *  error-throwing stubs.
 *
 *   params : void.
 *  returns : Returns (true) if set as new console driver, (false) otherwise.
 */
{
    __boolean retVal = ( (__getInitFlags() & INITFLAG_DISABLE_CONSOLE) ?
                         true : false );

    if (retVal == true)
        memcpy(entries, &noConEntries, sizeof (__vbConsoleEntries));

    return(retVal);
} /* __initNoConsole */

/* end of NoConsole.c ... */

