/*
 * BASIC string declarations...
 *
 *   Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICSTRING_H_
#define _INCLUDE_BASICSTRING_H_

#include "Boolean.h"
#include "RegState.h"

typedef struct
{
    unsigned int length;
    unsigned char *data;      /* THIS IS -=NOT=- NULL-TERMINATED! */
    boolean fixedLength;
} BasicString;

typedef BasicString *PBasicString;

PBasicString __allocString(STATEPARAMS, int length, boolean fixedLength);
PBasicString __createString(STATEPARAMS, char *asciz, boolean fixedLength);
PBasicString __constString(STATEPARAMS, char *asciz);
void __freeString(STATEPARAMS, PBasicString pBasicStr);
void __assignString(STATEPARAMS, PBasicString *ppBasicStr,
                    PBasicString pStrToAssign);
void __catString(STATEPARAMS, PBasicString *ppBasicStr, PBasicString pStrToCat);
char *__basicStringToAsciz(STATEPARAMS, PBasicString pStr);

#endif

/* end of BasicString.h ... */


