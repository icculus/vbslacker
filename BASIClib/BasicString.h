/*
 * BASIC string declarations...
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_BASICSTRING_H_
#define _INCLUDE_BASICSTRING_H_

typedef struct
{
    unsigned int length;
    __byte *data;          /* THIS IS -=NOT=- NULL-TERMINATED! */
    __boolean fixedLength;
} BasicString;

typedef BasicString *PBasicString;

PBasicString __allocString(int length, __boolean isFixed);
PBasicString __createString(char *asciz, __boolean fixedLength);
PBasicString __constString(char *asciz);
PBasicString __assignString(PBasicString to, PBasicString from);
PBasicString __catString(PBasicString str1, PBasicString str2);
char *__basicStringToAsciz(PBasicString pStr);

#endif /* _INCLUDE_BASICSTRING_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of BasicString.h ... */

