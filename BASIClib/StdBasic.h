/*
 * Include this header from all of your BASIClib interfacing code to get
 *  the minimum set of declarations needed for API programming/interfacing.
 *
 * BasicLib.h includes all this and more.
 *
 * This guarantees that the compiler will understand "boolean",
 *  "STATEPARAMS", etc...include in from your module's header, and then
 *  include your header in your module. This guarantees transparency, and
 *  the compiler always has access to the details it needs. Multiple includes
 *  of this file are karmically cool.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_
#define _INCLUDE_STDBASIC_H_

#include "Language.h"
#include "RegState.h"
#include "Boolean.h"
#include "BasicString.h"
#include "Variant.h"
#include "OnOffStopType.h"
#include "InternalMemManager.h"
#include "BasicError.h"
#include "Threads.h"
#include "Initialize.h"
#include "BasicArray.h"
#include "BasicFileStream.h"
#include "SignalHandlers.h"
#include "OnEvents.h"

#endif

/* end of StdBasic.h ... */

