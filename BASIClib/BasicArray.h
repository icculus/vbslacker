/*
 * Definition of a BASIC array.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_BASICARRAY_H_
#define _INCLUDE_BASICARRAY_H_

typedef struct
{
    void *data;                 /* Points to C array of variant structs. */
    long lBound;                /* Start of index range.            */
    long uBound;                /* End of index range.              */
    __boolean multiDimensional; /* An array of arrays?              */
    __boolean isStatic;         /* true == static, false == dynamic */
} __BasicArray;

typedef __BasicArray *__PBasicArray;

#endif  /* !defined _INCLUDE_BASICARRAY_H_ */
#endif  /* !defined _INCLUDE_STDBASIC_H_ */

/* end of BasicArray.h ... */

