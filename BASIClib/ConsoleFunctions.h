/*
 * Header file for BASIClib console routines.
 *
 *   Copyright (c) 1999 Ryan Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_CONSOLEFUNCTIONS_H_
#define _INCLUDE_CONSOLEFUNCTIONS_H_

#include "StdBasic.h"

extern void (*__getConsoleHandlerName)(char *buf, int size);
extern void (*__deinitConsoleHandler)(void);
extern void (*__printNewLine)(void);
extern void (*__printNChars)(char *str, int n);
extern void (*vbpii_viewPrint)(int top, int bottom);
extern void (*vbp_viewPrint)(void);
extern void (*vbp_cls)(void);
extern int  (*vbi_csrline)(void);
extern int  (*vbia_pos)(void *pVar);
extern void (*vbpiii_color)(int fore, int back, int bord);
extern void (*vbpil_color)(int fore, long palette);
extern void (*vbpi_color)(int fore);

void __initConsoleFunctions(void);
void __deinitConsoleFunctions(void);

void vbpV_print(PVariant pVar);
void vbpS_print(PBasicString str);
void __printAsciz(char *str);

#endif /* _INCLUDE_CONSOLEFUNCTIONS_H_ */

/* end of ConsoleFunctions.h ... */

