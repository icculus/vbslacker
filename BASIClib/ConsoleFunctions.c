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
void (*__vbpS_print)(STATEPARAMS, PBasicString x) = NULL;
void (*__vbpii_viewPrint)(STATEPARAMS, int top, int bottom) = NULL;
void (*__vbp_viewPrint)(STATEPARAMS) = NULL;
void (*__vbp_cls)(STATEPARAMS) = NULL;
int  (*__vbi_csrline)(STATEPARAMS) = NULL;
int  (*__vbiA_pos)(STATEPARAMS, void *pVar) = NULL;
void (*__vbpiii_color)(STATEPARAMS, int fore, int back, int bord) = NULL;
void (*__deinitConsole)(STATEPARAMS);


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
    __deinitConsole(STATEARGS);   /* call handler-specific de-init. */

    __vbpS_print = NULL;        /* blank all the func pointers out... */
    __vbpii_viewPrint = NULL;
    __vbp_viewPrint = NULL;
    __vbp_cls = NULL;
    __vbi_csrline = NULL;
    __vbiA_pos = NULL;
    __vbpiii_color = NULL;
    __deinitConsole = NULL;
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

