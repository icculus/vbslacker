/*
 * Header file for BASIClib console routines.
 *
 *   Copyright (c) 1998 Ryan Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_CONSOLEFUNCTIONS_H_
#define _INCLUDE_CONSOLEFUNCTIONS_H_

#include "StdBasic.h"

void __initConsoleFunctions(STATEPARAMS);
void __deinitConsoleFunctions(STATEPARAMS);
void vbpV_print(STATEPARAMS, PVariant pVar);
void vbpii_viewPrint(STATEPARAMS, int topRow, int bottomRow);
void vbp_viewPrint(STATEPARAMS);
void vbp_cls(STATEPARAMS);
int vbi_csrline(STATEPARAMS);
int vbiA_pos(STATEPARAMS, void *pVar);

extern void (*__deinitConsole)(STATEPARAMS);
extern void (*__vbpS_print)(STATEPARAMS, PBasicString x);
extern void (*__vbpii_viewPrint)(STATEPARAMS, int top, int bottom);
extern void (*__vbp_viewPrint)(STATEPARAMS);
extern void (*__vbp_cls)(STATEPARAMS);
extern int  (*__vbi_csrline)(STATEPARAMS);
extern int  (*__vbiA_pos)(STATEPARAMS, void *pVar);
extern void (*__vbpiii_color)(STATEPARAMS, int fore, int back, int bord);

#endif

/* end of ConsoleFunctions.h ... */

