/*
 * BASIC support library: conversion functions header.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_CONVERSIONFUNCTIONS_H_
#define _INCLUDE_CONVERSIONFUNCTIONS_H_

#include "RegState.h"
#include "BasicString.h"
#include "Variant.h"

#define cdbl(var) ((double) var)
#define cint(var) ((int) var)
#define clng(var) ((long) var)
#define csng(var) ((float) var)

int vbiS_asc(STATEPARAMS, PBasicString pBasicStr);
PBasicString vbSd_chr_DC_(STATEPARAMS, double asciiValue);
PBasicString vbSd_str_DC_(STATEPARAMS, double numeric);
double vbdS_val(STATEPARAMS, PBasicString pBasicStr);
PBasicString vbSd_hex_DC_(STATEPARAMS, double x);
PBasicString vbSd_oct_DC_(STATEPARAMS, double x);
int __VariantToInt(STATEPARAMS, PVariant var);
long __VariantToLong(STATEPARAMS, PVariant var);
float __VariantToFloat(STATEPARAMS, PVariant var);
double __VariantToDouble(STATEPARAMS, PVariant var);
PBasicString __VariantToString(STATEPARAMS, PVariant pVar, boolean byRef);

#endif

/* end of ConversionFunctions.h ... */


