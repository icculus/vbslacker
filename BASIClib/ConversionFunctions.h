/*
 * BASIC support library: conversion functions header.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_CONVERSIONFUNCTIONS_H_
#define _INCLUDE_CONVERSIONFUNCTIONS_H_

#include "BasicString.h"
#include "Variant.h"

unsigned int asc(PBasicString pBasicStr);
PBasicString chr_DC_(double asciiValue);
PBasicString str_DC_(double numeric);
double val(PBasicString pBasicStr);
PBasicString hex_DC_(double x);
PBasicString oct_DC_(double x);
int __VariantToInt(PVariant var);
long __VariantToLong(PVariant var);
float __VariantToFloat(PVariant var);
double __VariantToDouble(PVariant var);
PBasicString __VariantToString(PVariant pVar, boolean byRef);

#endif

/* end of ConversionFunctions.h ... */


