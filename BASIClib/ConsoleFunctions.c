/*
 * Console functions for BASIClib.
 *
 *    Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "ConsoleFunctions.h"

    /* Console drivers... */
#include "DirectConsole.h"
#include "NoConsole.h"
#include "CursesConsole.h"
#include "RedirectedConsole.h"

#warning Need structure for driver entry points!

/* internal function declarations needed at start... */
static void __preinitPrintNewLine(void);
static void __preinitPrintNChars(__byte *str, __long n);

/* variable function pointers... */
void (*__getConsoleHandlerName)(__byte *buf, __integer size) = NULL;
void (*__deinitConsoleHandler)(void) = NULL;
void (*__printNewLine)(void) = __preinitPrintNewLine;
void (*__printNChars)(__byte *str, __long n) = __preinitPrintNChars;
void (*_vbpii_viewPrint)( __integer top, __integer bottom) = NULL;
void (*_vbp_viewPrint)(void) = NULL;
void (*_vbp_cls)(void) = NULL;
__integer (*_vbi_csrline)(void) = NULL;
__integer (*_vbia_pos)(void *pVar) = NULL;
void (*_vbpiii_color)(__integer fore, __integer back, __integer bord) = NULL;
void (*_vbpil_color)(__integer fore, __long palette) = NULL;
void (*_vbpi_color)(__integer fore) = NULL;
void (*_vbpii_locate)(__integer newY, __integer newX) = NULL;
void (*_vbpNi_locate)(__integer newX) = NULL;
void (*_vbpiN_locate)(__integer newY) = NULL;
void (*_vbp_locate)(void) = NULL;


void __initConsoleFunctions(void)
/*
 * Run down the possible console handlers, in order of precedence,
 *  until we either find one that works, or we run out of options.
 *  If we run out of options, we force the redirected console to init,
 *  which performs simple output to stdout.
 *
 *      params : void.
 *     returns : void.
 */
{
    if (__initNoConsole() == true);
    else if (__initRedirectedConsole() == true);
    else if (__initDirectConsole() == true);
    else if (__initCursesConsole() == true);
    else __forceRedirectedConsole();
} /* __initConsoleFunctions */


void __deinitConsoleFunctions(void)
{
    __deinitConsoleHandler();   /* call handler-specific de-init. */

    __getConsoleHandlerName = NULL;  /* blank all the func pointers out... */
    __deinitConsoleHandler = NULL;
    __printNewLine = __preinitPrintNewLine;
    __printNChars = __preinitPrintNChars;
    _vbpii_viewPrint = NULL;
    _vbp_viewPrint = NULL;
    _vbp_cls = NULL;
    _vbi_csrline = NULL;
    _vbia_pos = NULL;
    _vbpiii_color = NULL;
    _vbpil_color = NULL;
    _vbpi_color = NULL;
    _vbpii_locate = NULL;
    _vbpNi_locate = NULL;
    _vbpiN_locate = NULL;
    _vbp_locate = NULL;
} /* __deinitConsoleFunctions */


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
    __printNChars(str->data, str->length);
} /* _vbpS_print */


void __printAsciz(__byte *str)
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
    __printNChars(str, (__long) strlen(str));
} /* __printAsciz */


static void __preinitPrintNChars(__byte *str, __long n)
/*
 * This function exists to allow initialization functions to
 *  write to stderr (under the guise of a full console driver)
 *  before a console driver has been selected and initialized.
 *  This is mostly for when there are runtime errors at startup.
 *
 *    params : str == pointer to chars to print.
 *             n == number of chars at (str) to print.
 *   returns : void.
 */
{
    __long i;

    for (i = 0; i < n; i++)
        fputc(str[i], stderr);
} /* __preinitPrintNChars */


static void __preinitPrintNewLine(void)
/*
 * See __preinitPrintNChars(), above.
 *
 *     params : void.
 *    returns : void.
 */
{
    fprintf(stderr, EOL_STRING);
} /* __preinitPrintNewLine */


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

