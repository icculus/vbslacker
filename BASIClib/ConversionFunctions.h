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

int vbiS_asc(PBasicString pBasicStr);
PBasicString vbSi_chr_DC_(int asciiValue);
PBasicString vbSd_str_DC_(double numeric);
double vbdS_val(PBasicString pBasicStr);
PBasicString vbSl_hex_DC_(long x);
PBasicString vbSl_oct_DC_(long x);
PBasicString vbS_mki_DC_(int num);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_CONVERSIONFUNCTIONS_H_ */

/* end of ConversionFunctions.h ... */


