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

PBasicString _vbSSl_right_DC_(PBasicString pStr, __long count);
PBasicString _vbSSl_left_DC_(PBasicString pStr, __long count);
PBasicString _vbSS_rtrim_DC_(PBasicString pStr);
PBasicString _vbSS_ltrim_DC_(PBasicString pStr);
PBasicString _vbSS_lcase_DC_(PBasicString pStr);
PBasicString _vbSS_ucase_DC_(PBasicString pStr);
__long _vblS_len(PBasicString pStr);
PBasicString _vbSl_space_DC_(__long length);
__long _vbllSS_instr(__long start, PBasicString str1, PBasicString str2);
__long _vblSS_instr(PBasicString str1, PBasicString str2);
PBasicString _vbSli_string_DC_(__long rep, __integer ch);
PBasicString _vbSlS_string_DC_(__long rep, PBasicString strCh);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_STRINGFUNCTIONS_H_ */

/* end of StringFunctions.h ... */


