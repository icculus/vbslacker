/*
 * Console functions for BASIClib.
 *
 *    Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#include "ConsoleFunctions.h"
#include "DirectConsole.h"
#include "NoConsole.h"
#include "CursesConsole.h"
#include "RedirectedConsole.h"

static boolean inGraphicsState = false;

/* variable function pointers... */
void (*__getConsoleHandlerName)(STATEPARAMS, char *buf, int size);
void (*__deinitConsoleHandler)(STATEPARAMS);
void (*vbpS_print)(STATEPARAMS, PBasicString x) = NULL;
void (*vbpii_viewPrint)(STATEPARAMS, int top, int bottom) = NULL;
void (*vbp_viewPrint)(STATEPARAMS) = NULL;
void (*vbp_cls)(STATEPARAMS) = NULL;
int  (*vbi_csrline)(STATEPARAMS) = NULL;
int  (*vbiA_pos)(STATEPARAMS, void *pVar) = NULL;
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
    else if (__initDirectConsole(STATEARGS) == true);
    else if (__initRedirectedConsole(STATEARGS) == true);
    else if (__initCursesConsole(STATEARGS) == true);
    else __forceRedirectedConsole(STATEARGS);
} /* __initConsoleFunctions */


void __deinitConsoleFunctions(STATEPARAMS)
{
    __deinitConsoleHandler(STATEARGS);   /* call handler-specific de-init. */

    __getConsoleHandlerName = NULL;  /* blank all the func pointers out... */
    __deinitConsoleHandler = NULL;
    vbpS_print = NULL;        
    vbpii_viewPrint = NULL;
    vbp_viewPrint = NULL;
    vbp_cls = NULL;
    vbi_csrline = NULL;
    vbiA_pos = NULL;
    vbpiii_color = NULL;
} /* __deinitConsoleFunctions */



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

