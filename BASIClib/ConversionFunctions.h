/*
 * BASIC support library: conversion functions header.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_CONVERSIONFUNCTIONS_H_
#define _INCLUDE_CONVERSIONFUNCTIONS_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

__integer _vbiS_asc(PBasicString pBasicStr);
PBasicString _vbSi_chr_DC_(__integer asciiValue);
PBasicString _vbSd_str_DC_(__double numeric);
__double _vbdS_val(PBasicString pBasicStr);
PBasicString _vbSl_hex_DC_(__long x);
PBasicString _vbSl_oct_DC_(__long x);
PBasicString _vbS_mki_DC_(__integer num);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_CONVERSIONFUNCTIONS_H_ */

/* end of ConversionFunctions.h ... */


