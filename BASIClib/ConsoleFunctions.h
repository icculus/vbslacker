/*
 * Header file for BASIClib console routines.
 *
 *   Copyright (c) 1998 Ryan Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_CONSOLEFUNCTIONS_H_
#define _INCLUDE_CONSOLEFUNCTIONS_H_

#include "RegState.h"

void __initConsole(STATEPARAMS)
void __deinitConsole(STATEPARAMS)
void vbpV_print(STATEPARAMS, PVariant pVar)
void vbpii_ViewPrint(STATEPARAMS, int topRow, int bottomRow)
void vbp_ViewPrint(STATEPARAMS)
void vbp_cls(STATEPARAMS)
int vbi_csrline(STATEPARAMS)
int vbiV_pos(STATEPARAMS, PVariant pVar)

#endif

/* end of ConsoleFunctions.h ... */

