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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void *data;                   /* Points to C array of variant structs. */
    __long lBound;                /* Start of index range.            */
    __long uBound;                /* End of index range.              */
    __boolean multiDimensional;   /* An array of arrays?              */
    __boolean isStatic;           /* true == static, false == dynamic */
} __BasicArray;

typedef __BasicArray *__PBasicArray;

#ifdef __cplusplus
}
#endif

#endif  /* !defined _INCLUDE_BASICARRAY_H_ */
#endif  /* !defined _INCLUDE_STDBASIC_H_ */

/* end of BasicArray.h ... */

