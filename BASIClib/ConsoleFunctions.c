/*
 * Console functions for BASIClib.
 *
 *    Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicConsole.h"

void _vbpV_print(PVariant pVar)
/*
 * Take a variant, convert to a string, and print it.
 *
 *   params : pVar == variable to print.
 *  returns : void.
 */
{
    PBasicString str;

    str = __variantToString(pVar, true);
    _vbpS_print(str);
/*    __freeString(str);*/
} /* _vbpV_print */


void _vbpS_print(PBasicString str)
/*
 * Print a BASIC string to the console. NULL characters do not
 *  terminate the string, and are acceptable for printing.
 *
 *     params : str == BASIC string to print.
 *    returns : void.
 */
{
    consoleEntries.__printNChars(str->data, str->length);
} /* _vbpS_print */


void __printAsciz(const __byte *str)
/*
 * Print a null-terminated C string (ASCIZ string) to the console.
 *  If there is a null character in the string, it is considered the
 *  end, even if you have more data, so be careful with this. Use
 *  either __printNChars() or vbpS_print() in such a case.
 *
 *      params : str == ASCIZ string to print.
 *     returns : void.
 */
{
    consoleEntries.__printNChars(str, (__long) strlen(str));
} /* __printAsciz */


void _vbpif_sound(__integer frequency, __single duration)
{
    /* These are the valid ranges according to VBDOS 1.0 ... */
    if (frequency < 37)
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);

    if ((duration < 0.0) || (duration > 65535.0))
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);

    consoleEntries._vbpif_sound(frequency, duration);
} /* _vbpil_sound */


void __getConsoleDriverName(__byte *buf, __integer size)
{
    consoleEntries.__getConsoleDriverName(buf, size);
} /* __getConsoleDriverName */

void __printNewLine(void)
{
    consoleEntries.__printNewLine();
} /* __printNewLine */

void __printNChars(const __byte *str, __long n)
{
    consoleEntries.__printNChars(str, n);
} /* __printNChars */

void _vbpii_viewPrint(__integer top, __integer bottom)
{
    consoleEntries._vbpii_viewPrint(top, bottom);
} /* _vbpii_viewPrint */

void _vbp_viewPrint(void)
{
    consoleEntries._vbp_viewPrint();
} /* _vbp_viewPrint */

void _vbp_cls(void)
{
    consoleEntries._vbp_cls();
} /* _vbp_cls */

__integer _vbi_csrline(void)
{
    return(consoleEntries._vbi_csrline());
} /* _vbi_csrline */

__integer _vbia_pos(void *pVar)
{
    return(consoleEntries._vbia_pos(pVar));
} /* _vbia_pos */

void _vbpiii_color(__integer fore, __integer back, __integer bord)
{
    consoleEntries._vbpiii_color(fore, back, bord);
} /* _vbpiii_color */

void _vbpil_color(__integer fore, __long palette)
{
    consoleEntries._vbpil_color(fore, palette);
} /* _vbpil_color */

void _vbpi_color(__integer fore)
{
    consoleEntries._vbpi_color(fore);
} /* _vbpi_color */

void _vbpii_locate(__integer newY, __integer newX)
{
    consoleEntries._vbpii_locate(newY, newX);
} /* _vbpii_locate */

void _vbpNi_locate(__integer newX)
{
    consoleEntries._vbpNi_locate(newX);
} /* vbpNi_locate */

void _vbpiN_locate(__integer newY)
{
    consoleEntries._vbpiN_locate(newY);
} /* vbpiN_locate */

void _vbp_locate(void)
{
    consoleEntries._vbp_locate();
} /* _vbp_locate */

void _vbp_beep(void)
{
    consoleEntries._vbp_beep();
} /* _vbp_beep */

#if 0
void _vbp1ss1ssi2i_line(__boolean step1, __single x1, __single y1,
                        __boolean step2, __single x2, __single y2,
                        __integer color, __vbLineType typ, __integer style)
{
} /* _vbp1ss1ssi2i_line */

void _vbp1ssi2i_line(__boolean step2, __single x2, __single y2,
                     __integer color, __vbLineType typ, __integer style)
{
    _vbp1ss1ssi2i_line(false, lastX, lastY, step2, x2, y2, color, typ, style);
} /* _vbp1ssi2i_line */

void _vbp1ssi2i_line(__boolean step2, __single x2, __single y2,
                     __integer color, __vbLineType typ, __integer style)
{
    _vbp1ss1ssi2i_line(false, lastX, lastY, step2, x2, y2, color, typ, style);
} /* _vbp1ssi2i_line */
#endif




/* qbcolor */
/* color */
/* write */
/* input */
/* line input */
/* INKEY$ */
/* width */
/* key */
/* print using */
/* tab */
/* beep */

/*  -- graphics ... -- */
/* circle */
/* draw */
/* get */
/* put */
/* screen */
/* line */
/* paint */
/* point */
/* palette */
/* pcopy */
/* pset */
/* preset */
/* pmap */
/* view */
/* window */


/* end of ConsoleFunctions.c ... */

