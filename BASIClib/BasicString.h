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

PBasicString __allocString(STATEPARAMS, int length, __boolean isFixed);
PBasicString __createString(STATEPARAMS, char *asciz, __boolean fixedLength);
PBasicString __constString(STATEPARAMS, char *asciz);
PBasicString __assignString(STATEPARAMS, PBasicString to, PBasicString from);
PBasicString __catString(STATEPARAMS, PBasicString str1, PBasicString str2);
char *__basicStringToAsciz(STATEPARAMS, PBasicString pStr);

#endif /* _INCLUDE_BASICSTRING_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of BasicString.h ... */

