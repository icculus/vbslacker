/*
 * Definition of a BASIC array.
 *
 *   Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_BASICARRAY_H_
#define _INCLUDE_BASICARRAY_H_

#include "StdBasic.h"

typedef struct
{
    void *data;     /* Points to C array.    */
    long start;     /* Start of index range. */
    long end;       /* End of index range.   */
} BasicArray;

#endif
#endif

/* end of BasicArray.h ... */

