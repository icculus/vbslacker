/*
 * BASIClib string-related user functions header. For the more low-level
 *  BasicString manipulation routines, such as __createString(),
 *  refer to BasicString.h ...
 *
 * Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STRINGFUNCTIONS_H_
#define _INCLUDE_STRINGFUNCTIONS_H_

#include "RegState.h"

/* !!!
mid$
mid$ =
*/

PBasicString right_DC_(STATEPARAMS, PBasicString pStr, int count);
PBasicString left_DC_(STATEPARAMS, PBasicString pStr, int count);
PBasicString rtrim_DC_(STATEPARAMS, PBasicString pStr);
PBasicString ltrim_DC_(STATEPARAMS, PBasicString pStr);
PBasicString lcase_DC_(STATEPARAMS, PBasicString pStr);
PBasicString ucase_DC_(STATEPARAMS, PBasicString pStr);
PBasicString space_DC_(STATEPARAMS, int length);
int len(STATEPARAMS, PBasicString pStr);

#endif

/* end of StringFunctions.h ... */


