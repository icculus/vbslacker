/*
 * Definition of a BASIC array.
 *
 *   Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICARRAY_H_
#define _INCLUDE_BASICARRAY_H_

typedef struct
{
    void *data;     /* Points to C array.    */
    long start;     /* Start of index range. */
    long end;       /* End of index range.   */
} BasicArray;

#endif

/* end of BasicArray.h ... */

