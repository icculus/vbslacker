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

extern void (*__getConsoleHandlerName)(STATEPARAMS, char *buf, int size);
extern void (*__deinitConsoleHandler)(STATEPARAMS);
extern void (*__printNewLine)(STATEPARAMS);
extern void (*__printNChars)(STATEPARAMS, char *str, int n);
extern void (*vbpii_viewPrint)(STATEPARAMS, int top, int bottom);
extern void (*vbp_viewPrint)(STATEPARAMS);
extern void (*vbp_cls)(STATEPARAMS);
extern int  (*vbi_csrline)(STATEPARAMS);
extern int  (*vbia_pos)(STATEPARAMS, void *pVar);
extern void (*vbpiii_color)(STATEPARAMS, int fore, int back, int bord);
extern void (*vbpil_color)(STATEPARAMS, int fore, long pallette);
extern void (*vbpi_color)(STATEPARAMS, int fore);

void vbpV_print(STATEPARAMS, PVariant pVar)
void vbpS_print(STATEPARAMS, PBasicString str)
void __printAsciz(STATEPARAMS, char *str)

#endif

/* end of ConsoleFunctions.h ... */

