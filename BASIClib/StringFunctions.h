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

PBasicString vbSSi_right_DC_(STATEPARAMS, PBasicString pStr, int count);
PBasicString vbSSi_left_DC_(STATEPARAMS, PBasicString pStr, int count);
PBasicString vbSS_rtrim_DC_(STATEPARAMS, PBasicString pStr);
PBasicString vbSS_ltrim_DC_(STATEPARAMS, PBasicString pStr);
PBasicString vbSS_lcase_DC_(STATEPARAMS, PBasicString pStr);
PBasicString vbSS_ucase_DC_(STATEPARAMS, PBasicString pStr);
PBasicString vbSi_space_DC_(STATEPARAMS, int length);
int vbiS_len(STATEPARAMS, PBasicString pStr);

#endif

/* end of StringFunctions.h ... */


