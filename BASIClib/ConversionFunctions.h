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

int asc(STATEPARAMS, PBasicString pBasicStr);
PBasicString chr_DC_(STATEPARAMS, double asciiValue);
PBasicString str_DC_(STATEPARAMS, double numeric);
double val(STATEPARAMS, PBasicString pBasicStr);
PBasicString hex_DC_(STATEPARAMS, double x);
PBasicString oct_DC_(STATEPARAMS, double x);
int __VariantToInt(STATEPARAMS, PVariant var);
long __VariantToLong(STATEPARAMS, PVariant var);
float __VariantToFloat(STATEPARAMS, PVariant var);
double __VariantToDouble(STATEPARAMS, PVariant var);
PBasicString __VariantToString(STATEPARAMS, PVariant pVar, boolean byRef);

#endif

/* end of ConversionFunctions.h ... */


