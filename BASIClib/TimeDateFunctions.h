/*
 * Declarations for time/date functions for BASIC library.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_TIMEDATEFUNCTIONS_H_
#define _INCLUDE_TIMEDATEFUNCTIONS_H_

#include "RegState.h"
#include "BasicString.h"

typedef enum {ON, OFF, STOP} TimerArgEnum;

void __initTimeDateFunctions(STATEPARAMS);

double func_timer(STATEPARAMS);
void proc_timer(STATEPARAMS, TimerArgEnum setting);

PBasicString func_time_DC_(STATEPARAMS);
void proc_time_DC_(STATEPARAMS, PBasicString newTimeStr);

PBasicString func_date_DC_(STATEPARAMS);
void proc_date_DC_(STATEPARAMS, PBasicString newTimeStr);

long dateserial(STATEPARAMS, int year, int month, int day);
long datevalue(STATEPARAMS, PBasicString date);
int day(STATEPARAMS, long serialNum);
int weekday(STATEPARAMS, long serialNum);
int month(STATEPARAMS, long serialNum);
int year(STATEPARAMS, long serialNum);

int now(STATEPARAMS);

#endif

/* end of TimeDateFunctions.h ... */


