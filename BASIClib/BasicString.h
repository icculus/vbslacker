/*
 * BASIC string declarations...
 *
 *   Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICSTRING_H_
#define _INCLUDE_BASICSTRING_H_

#include "Boolean.h"

typedef struct
{
    unsigned int length;
    char *data;             /* THIS IS -=NOT=- NULL-TERMINATED! */
    boolean fixedLength;
} BasicString;

typedef BasicString *PBasicString;

void __allocString(int length, boolean fixedLength);
void __createString(char *asciz, boolean fixedLength);
void __cloneString(PBasicString pBasicStr);
void __freeString(PBasicString pBasicStr);

#endif

/* end of BasicString.h ... */


