/*
 * BASIC support library: conversion functions header.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_CONVERSIONFUNCTIONS_H_
#define _INCLUDE_CONVERSIONFUNCTIONS_H_

#include "StdBasic.h"

#define cdbl(var) ((double) var)
#define cint(var) ((int) var)
#define clng(var) ((long) var)
#define csng(var) ((float) var)

int vbiS_asc(STATEPARAMS, PBasicString pBasicStr);
PBasicString vbSi_chr_DC_(STATEPARAMS, int asciiValue);
PBasicString vbSd_str_DC_(STATEPARAMS, double numeric);
double vbdS_val(STATEPARAMS, PBasicString pBasicStr);
PBasicString vbSl_hex_DC_(STATEPARAMS, long x);
PBasicString vbSl_oct_DC_(STATEPARAMS, long x);

#endif

/* end of ConversionFunctions.h ... */


