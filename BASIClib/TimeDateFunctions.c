/*
 * Time/date functions for BASIC library.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include "TimeDateFunctions.h"


#ifdef WIN32
/* !!! cygwin32 has these functions, but no prototypes.
 * !!!  remove this when they do.
 */
    int gettimeofday(struct timeval *tp, struct timezone *tzp);
    int settimeofday(const struct timeval *tp, const struct timezone *tzp);
#endif


#define SECONDS_IN_A_DAY 86400

    /* to save every byte of storage, these are stored as chars. */
static char daysInMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
static char daysInLeapYearMonth[12] = {31,29,31,30,31,30,31,31,30,31,30,31};
static time_t todaySecs;


static struct tm *__getBrokenTime(void)
/*
 * Returns brokendown time for current time of day, a glibc-specific
 *  data structure that parses time into various fields, like day,
 *  month, hour, seconds, etc...certain glibc time/date functions will
 *  overwrite the static buffer that the return value points to, so
 *  don't keep this value around too long, or it may not prove correct.
 *
 *    params : void.
 *   returns : ptr to struct tm for current time.
 */
{
    time_t currentTime;
    struct tm *retVal = NULL;
    
    if (time(&currentTime) == (time_t) -1)     /* can't retrieve time?! */
        __runtimeError(ERR_INTERNAL_ERROR);
    else
        retVal = localtime(&currentTime);

    return(retVal);
} /* __getBrokenTime */


static time_t __getTodaySecs(void)
/*
 * Figure out how many seconds have elasped between Midnight Jan, 1970,
 *  GMT, and the beginning of today. Calculating this once will help the
 *  efficiency of func_timer()...
 *
 *   params : void.
 *  returns : see above.
 */
{
    struct tm *brokenTime = __getBrokenTime();

    brokenTime->tm_sec  = 0;    /* reset time to beggining of day. */
    brokenTime->tm_min  = 0;
    brokenTime->tm_hour = 0;

    return(mktime(brokenTime));  /* struct to long int seconds and return. */
} /* __getTodaySecs */


void __initTimeDateFunctions(void)
{
    todaySecs = __getTodaySecs();
} /* __initTimeDateFunctions */


__single _vbf_timer(void)
/*
 * Returns the number of seconds elasped since midnight, with two
 *  decimal places worth of precision.
 *
 *   params : void.
 *  returns : see above.
 */
{
    struct timeval currentTime;  /* high resolution time. */
    double retVal;

    gettimeofday(&currentTime, NULL);
    retVal = difftime(todaySecs, currentTime.tv_sec);

    if (retVal >= SECONDS_IN_A_DAY)     /* need to update todaySecs? */
    {
        retVal -= SECONDS_IN_A_DAY;
        todaySecs += SECONDS_IN_A_DAY;
    } /* if */

    retVal += ((currentTime.tv_usec % 100) / 100); /* Add 2 decimal places. */
    return(retVal);
} /* _vbf_timer */


static PBasicString __makeTimeDateString(__long size, const __byte *fmt)
/*
 * This called by the function versions of TIME$ and DATE$, since they
 *  require the exact same logic, with some different parameters.
 *
 *     params : size == maximum size for BASIC string to be.
 *              *fmt == formatting for returned BASIC string. This is
 *                      passed to strftime() (see <time.h>...).
 *    returns : a newly allocated BASIC string, formatted with current
 *               time or date as BASIC would format it.
 */
{
    PBasicString retVal = NULL;
    __byte buffer[size + 1];          /* plus one for NULL terminator. */
    int rc;
    struct tm *brokenTime = __getBrokenTime();

    rc = strftime(buffer, sizeof (buffer), fmt, brokenTime);

#warning !!! check __makeTimeDateString()...

    /* !!! == ? !!! */
    if ((unsigned int) rc == sizeof (buffer))  /* not enough space? */
        __runtimeError(ERR_INTERNAL_ERROR);
    else
        retVal = __createString(buffer, false);

    return(retVal);
} /* __makeTimeDateString */



PBasicString _vbS_time_DC_(void)
/*
 * Returns a BASIC string containing the current system time in the format
 *  "HH:MM:SS" (8 bytes) ... The time is returned in 24-hour format.
 *
 *     params : void.
 *    returns : newly allocated BASIC string containing current system time.
 */
{
    return(__makeTimeDateString(8, "%H:%M:%S"));
} /* _vbS_time_DC_ */


void _vbpS_time_DC_(PBasicString newTimeStr)
{
#warning write me!
    /* !!! compare to proc_date_DC_()... */
} /* _vbpS_time_DC_ */


PBasicString _vbS_date_DC_(void)
/*
 * Return a BASIC string containing current date in the
 *  format "MM-DD-YYYY" (10 bytes).
 *
 *    params : void.
 *   returns : newly allocated BASIC string with current date.
 */
{
    return(__makeTimeDateString(10, "%m-%d-%C%y"));
} /* _vbS_date_DC_ */


static __boolean __checkDateBounds(__integer month,
                                   __integer day,
                                   __integer year)
/*
 * Verify that specified numbers are valid for setting through BASIC's
 *  DATE$ function. The bounds appear to be:
 *
 *  !!! Check that year against Visual Basic. Qbasic only lets to 2099...
 *
 *   month : 01 - 12
 *     day : 01 - 31  (depending on the month and leap year.)
 *    year : 1980 - 2099
 *
 *  params : month, day, year == see above.
 * returns : boolean true if all the numbers are acceptable, false otherwise.
 */
{
    __boolean retVal = true;
    __integer maxDays;

    if ((year < 1980) || (year > 2099))
        retVal = false;

    else if ((month < 1) || (month > 12))
        retVal = false;

    else
    {
        maxDays = (((year % 4) != 0) ? (int) daysInMonth[month - 1] :
                                       (int) daysInLeapYearMonth[month - 1]);

        if ((day < 1) || (day > maxDays))
            retVal = false;
    } /* else */

    return(retVal);
} /* __checkDateBounds */



static __boolean __prepareDateString(__byte *str)
/*
 * Check a date string for initial validity. This will check that characters
 *  are acceptable, but won't determine if numbers are in range.
 *
 *     params : *str == asciz string to prepare.
 *    returns : boolean true is data is in correct format, false otherwise.
 */
{
    __integer i;
    __integer sepCount = 0;
    __byte separator = '\0';
    __boolean retVal = true;
    __long max = strlen(str);

    for (i = 0; (i < max) && (retVal == true); i++)
    {
        if (!isdigit((int) str[i]))   /* not a number? */
        {
            if ((str[i] != '-') && (str[i] != '/'))
                retVal = false;
            else if ((separator != '\0') && (str[i] != separator))
                retVal = false;
            else
            {
                separator = str[i];
                sepCount++;
            } /* else */
        } /* if */
    } /* for */

    if (sepCount != 2)
        retVal = false;

    return(retVal);
} /* __prepareDateString */



static __boolean __setSystemDate(__integer month, __integer day,
                                 __integer year,  __long *pErrVal)
/*
 * Set system clock to new date.
 *
 *   params : month == 1 - 12.
 *            day   == 1 - 31.
 *            year  == 1980 - 2099  !!! correct?
 *  returns : boolean false on error. If error, *pErrVal will contain the
 *             appropriate BASIC runtime error. boolean true on success.
 */
{
    __boolean retVal = true;
    struct tm *brokenTime = __getBrokenTime();
    struct timeval tv;

    brokenTime->tm_mday = day;  
    brokenTime->tm_mon = month - 1;      /* "-1"...param is option base 0. */
    brokenTime->tm_mon = year - 1900;     /* tm_mon is years since 1900... */

    tv.tv_usec = 0;
    tv.tv_sec = mktime(brokenTime);
    if (tv.tv_sec == -1)
    {
        *pErrVal = ERR_INTERNAL_ERROR;
        retVal = false;
    } /* if */
    else
    {
        if (settimeofday(&tv, NULL) == -1)
        {
            retVal = false;
            if (errno == EPERM)     /* access denied. */
                *pErrVal = ERR_PERMISSION_DENIED;
            else
                *pErrVal = ERR_INTERNAL_ERROR;
        } /* if */
    } /* else */

    return(retVal);
} /* __setSystemDate */


void _vbpS_date_DC_(PBasicString newDateStr)
/*
 * Set the system date. This throws a BASIC runtime error if the process
 *  doesn't have rights to change the system clock. DOS and Windows versions
 *  of BASIC don't have this problem, since any moron can tinker with
 *  the clock on those boxes. In short, this is a mildly threatning
 *  variation from the standard, as most VB programmers don't bother to
 *  set up an error handler before setting the time.
 *
 *   params : newDateStr == a BASIC string containing the desired change
 *                           of date in one of these formats:
 *                           "mm-dd-yy", "mm-dd-yyyy",
 *                           "mm/dd/yy", "mm/dd/yyyy". Numbers need not
 *                           be padded with zeroes, but can be.
 *  returns : void.
 */
{
    __integer month;
    __integer day;
    __integer year;
    char str[newDateStr->length + 1];
    char *next;
    __long errVal;

    memcpy(str, newDateStr->data, newDateStr->length);
    str[newDateStr->length] = '\0';

    if (__prepareDateString(str) == false)
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else
    {
        month = strtol(str, &next, 10);
        day = strtol(next + 1, &next, 10);
        year = strtol(next + 1, NULL, 10);

        if (year < 100)
            year += 1900;

        if (__checkDateBounds(month, day, year) == false)
            __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);

        else if (__setSystemDate(month, day, year, &errVal) == false)
            __runtimeError(errVal);
    } /* else */
} /* _vbpS_date_DC_ */




/*
        !!! Yikes! I'll look into these later...

    long dateserial(int year, int month, int day);
    long datevalue(PBasicString date);
    int day(long serialNum);
    int weekday(long serialNum);
    int month(long serialNum);
    int year(long serialNum);
    int now(void);
*/


/* end of TimeDateFunctions.c ... */


