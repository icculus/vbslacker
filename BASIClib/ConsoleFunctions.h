/*
 * Header file for BASIClib console routines.
 *
 *   Copyright (c) 1999 Ryan Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_CONSOLEFUNCTIONS_H_
#define _INCLUDE_CONSOLEFUNCTIONS_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void (*__getConsoleHandlerName)(__byte *buf, __integer size);
extern void (*__deinitConsoleHandler)(void);
extern void (*__printNewLine)(void);
extern void (*__printNChars)(__byte *str, __long n);
extern void (*_vbpii_viewPrint)(__integer top, __integer bottom);
extern void (*_vbp_viewPrint)(void);
extern void (*_vbp_cls)(void);
extern __integer (*_vbi_csrline)(void);
extern __integer (*_vbia_pos)(void *pVar);
extern void (*_vbpiii_color)(__integer fore, __integer back, __integer bord);
extern void (*_vbpil_color)(__integer fore, __long palette);
extern void (*_vbpi_color)(__integer fore);
extern void (*_vbpii_locate)(__integer newY, __integer newX);
extern void (*_vbpNi_locate)(__integer newX);
extern void (*_vbpiN_locate)(__integer newY);
extern void (*_vbp_locate)(void);


void __initConsoleFunctions(void);
void __deinitConsoleFunctions(void);

void _vbpV_print(PVariant pVar);
void _vbpS_print(PBasicString str);
void __printAsciz(__byte *str);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_CONSOLEFUNCTIONS_H_ */

/* end of ConsoleFunctions.h ... */

