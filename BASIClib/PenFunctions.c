/*
 * Pen functions for BASIClib. Note that all of these are dummy functions,
 *  the same Qbasic would return if there was no light pen attached to the
 *  the system. Since Linux (and hell, Win95) have no specific light-pen
 *  APIs, the best we could do is emulate a light pen with the mouse, but
 *  that seems a little useless. These are just here for basic (er...VERY
 *  basic) compatibility with older BASICs...modern BASICs don't include
 *  this support at all.
 *
 *    Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "PenFunctions.h"


int vbii_pen(int arg)
{
    switch (arg)
    {
        case 0: return(0);
        case 1: return(0);
        case 2: return(0);
        case 3: return(0);
        case 4: return(0);
        case 5: return(0);
        case 6: return(1);
        case 7: return(1);
        case 8: return(1);
        case 9: return(1);
        default: __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    } /* switch */

    return(0);  /* should never hit this, but just in case... */
} /* vbii_pen */


void vbpO_pen(OnOffStopType onOffStop) {}


/* end of PenFunctions.c ... */

