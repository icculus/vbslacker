/*
 * Declarations for time/date functions for BASIC library.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_TIMEDATEFUNCTIONS_H_
#define _INCLUDE_TIMEDATEFUNCTIONS_H_

#include "StdBasic.h"

void __initTimeDateFunctions(void);
float vbf_timer(void);
void vbpO_timer(OnOffStopType setting);
PBasicString vbS_time_DC_(void);
void vbpS_time_DC_(PBasicString newTimeStr);
PBasicString vbS_date_DC_(void);
void vbpS_date_DC_(PBasicString newDateStr);

#endif /* _INCLUDE_TIMEDATEFUNCTIONS_H_ */

/* end of TimeDateFunctions.h ... */


