/*
 * BASIC string declarations...
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 *    This file written by Ryan C. Gordon.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_BASICSTRING_H_
#define _INCLUDE_BASICSTRING_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    __long length;
    __byte *data;          /* THIS IS -=NOT=- NULL-TERMINATED! */
    __boolean fixedLength;
} BasicString;

typedef BasicString *PBasicString;

PBasicString __allocString(__long length, __boolean isFixed);
PBasicString __createString(__byte *asciz, __boolean fixedLength);
PBasicString __constString(__byte *asciz);
PBasicString __assignString(PBasicString to, PBasicString from);
PBasicString __catString(PBasicString str1, PBasicString str2);
__byte *__basicStringToAsciz(PBasicString pStr);
void __freeString(PBasicString pStr);
__byte *__copyAscizString(__byte *copyThis);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_BASICSTRING_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of BasicString.h ... */

