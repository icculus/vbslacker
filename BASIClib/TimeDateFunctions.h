/*
 * Declarations for time/date functions for BASIC library.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_TIMEDATEFUNCTIONS_H_
#define _INCLUDE_TIMEDATEFUNCTIONS_H_

#include "StdBasic.h"

void __initTimeDateFunctions(STATEPARAMS);
float vbf_timer(STATEPARAMS);
void vbpO_timer(STATEPARAMS, OnOffStopType setting);
PBasicString vbS_time_DC_(STATEPARAMS);
void vbpS_time_DC_(STATEPARAMS, PBasicString newTimeStr);
PBasicString vbS_date_DC_(STATEPARAMS);
void vbpS_date_DC_(STATEPARAMS, PBasicString newDateStr);

#endif

/* end of TimeDateFunctions.h ... */


