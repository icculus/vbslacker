/*
 * Header file for internal BASIClib console routines.
 *
 *   Copyright (c) 1999 Ryan Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_BASICCONSOLE_H_
#define _INCLUDE_BASICCONSOLE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void (*__getConsoleDriverName)(__byte *buf, __integer size);
    void (*__deinitConsoleDriver)(void);
    void (*__printNewLine)(void);
    void (*__printNChars)(const __byte *str, __long n);
    void (*_vbpii_viewPrint)(__integer top, __integer bottom);
    void (*_vbp_viewPrint)(void);
    void (*_vbp_cls)(void);
    __integer (*_vbi_csrline)(void);
    __integer (*_vbia_pos)(void *pVar);
    void (*_vbpiii_color)(__integer fore, __integer back, __integer bord);
    void (*_vbpil_color)(__integer fore, __long palette);
    void (*_vbpi_color)(__integer fore);
    void (*_vbpii_locate)(__integer newY, __integer newX);
    void (*_vbpNi_locate)(__integer newX);
    void (*_vbpiN_locate)(__integer newY);
    void (*_vbp_locate)(void);
    void (*_vbpif_sound)(__integer freq, __single duration);
    void (*_vbp_beep)(void);
} __vbConsoleEntries;

extern __vbConsoleEntries consoleEntries;

void __initConsole(char *vbHomeDir);
void __deinitConsole(void);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_BASICCONSOLE_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of BasicConsole.h ... */

