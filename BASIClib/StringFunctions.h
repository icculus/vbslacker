/*
 * BASIClib string-related user functions header. For the more low-level
 *  BasicString manipulation routines, such as __createString(),
 *  refer to BasicString.h ...
 *
 * Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STRINGFUNCTIONS_H_
#define _INCLUDE_STRINGFUNCTIONS_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

PBasicString vbSSi_right_DC_(PBasicString pStr, int count);
PBasicString vbSSi_left_DC_(PBasicString pStr, int count);
PBasicString vbSS_rtrim_DC_(PBasicString pStr);
PBasicString vbSS_ltrim_DC_(PBasicString pStr);
PBasicString vbSS_lcase_DC_(PBasicString pStr);
PBasicString vbSS_ucase_DC_(PBasicString pStr);
int vbiS_len(PBasicString pStr);
PBasicString vbSi_space_DC_(int length);
int vbiiSS_instr(int start, PBasicString str1, PBasicString str2);
int vbiSS_instr(PBasicString str1, PBasicString str2);
PBasicString vbSii_string_DC_(int rep, int ch);
PBasicString vbSiS_string_DC_(int rep, PBasicString strCh);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_STRINGFUNCTIONS_H_ */

/* end of StringFunctions.h ... */


