/*
 * Console functions for BASIClib.
 *
 *    Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#include "ConsoleFunctions.h"

    /* Console drivers... */
#include "DirectConsole.h"
#include "NoConsole.h"
#include "CursesConsole.h"
#include "RedirectedConsole.h"

/* internal function declarations needed at start... */
static void __preinitPrintNewLine(STATEPARAMS);
static void __preinitPrintNChars(STATEPARAMS, char *str, int n);

/* variable function pointers... */
void (*__getConsoleHandlerName)(STATEPARAMS, char *buf, int size) = NULL;
void (*__deinitConsoleHandler)(STATEPARAMS) = NULL;
void (*__printNewLine)(STATEPARAMS) = __preinitPrintNewLine;
void (*__printNChars)(STATEPARAMS, char *str, int n) = __preinitPrintNChars;
void (*vbpii_viewPrint)(STATEPARAMS, int top, int bottom) = NULL;
void (*vbp_viewPrint)(STATEPARAMS) = NULL;
void (*vbp_cls)(STATEPARAMS) = NULL;
int  (*vbi_csrline)(STATEPARAMS) = NULL;
int  (*vbia_pos)(STATEPARAMS, void *pVar) = NULL;
void (*vbpiii_color)(STATEPARAMS, int fore, int back, int bord) = NULL;
void (*vbpil_color)(STATEPARAMS, int fore, long palette) = NULL;
void (*vbpi_color)(STATEPARAMS, int fore) = NULL;


void __initConsoleFunctions(STATEPARAMS)
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
    if (__initNoConsole(STATEARGS) == true);
    else if (__initRedirectedConsole(STATEARGS) == true);
    else if (__initDirectConsole(STATEARGS) == true);
    else if (__initCursesConsole(STATEARGS) == true);
    else __forceRedirectedConsole(STATEARGS);
} /* __initConsoleFunctions */


void __deinitConsoleFunctions(STATEPARAMS)
{
    __deinitConsoleHandler(STATEARGS);   /* call handler-specific de-init. */

    __getConsoleHandlerName = NULL;  /* blank all the func pointers out... */
    __deinitConsoleHandler = NULL;
    __printNewLine = __preinitPrintNewLine;
    __printNChars = __preinitPrintNChars;
    vbpii_viewPrint = NULL;
    vbp_viewPrint = NULL;
    vbp_cls = NULL;
    vbi_csrline = NULL;
    vbia_pos = NULL;
    vbpiii_color = NULL;
} /* __deinitConsoleFunctions */


void vbpV_print(STATEPARAMS, PVariant pVar)
/*
 * Take a variant, convert to a string, and print it.
 *
 *   params : pVar == variable to print.
 *  returns : void.
 */
{
    PBasicString str;

    str = __variantToString(STATEARGS, pVar, true);
    vbpS_print(STATEARGS, str);
    __freeString(STATEARGS, str);
} /* vbpV_print */


void vbpS_print(STATEPARAMS, PBasicString str)
/*
 * Print a BASIC string to the console. NULL characters do not
 *  terminate the string, and are acceptable for printing.
 *
 *     params : str == BASIC string to print.
 *    returns : void.
 */
{
    __printNChars(STATEARGS, str->data, str->length);
} /* vbpS_print */


void __printAsciz(STATEPARAMS, char *str)
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
    __printNChars(STATEARGS, str, strlen(str));
} /* __printAsciz */


static void __preinitPrintNChars(STATEPARAMS, char *str, int n)
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
    int i;

    for (i = 0; i < n; i++)
        fputc(str[i], stderr);
} /* __preinitPrintNChars */


static void __preinitPrintNewLine(STATEPARAMS)
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
/* locate */
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

