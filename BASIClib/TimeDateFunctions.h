/*
 * Declarations for time/date functions for BASIC library.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_TIMEDATEFUNCTIONS_H_
#define _INCLUDE_TIMEDATEFUNCTIONS_H_

typedef enum {ON, OFF, STOP} TimerArgEnum;

double func_timer(void);
void proc_timer(TimerArgEnum setting);

PBasicString func_time_DC_(void);
void proc_time_DC_(PBasicString newTimeStr);

PBasicString func_date_DC_(void);
void proc_date_DC_(PBasicString newTimeStr);

long dateserial(int year, int month, int day);
long datevalue(PBasicString date);
int day(long serialNum);
int weekday(long serialNum);
int month(long serialNum);
int year(long serialNum);

int now(void);

#endif

/* end of TimeDateFunctions.h ... */


