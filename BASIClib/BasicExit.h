/*
 * Macros for EXIT SUB, EXIT FUNCTION.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_BASICEXIT_H_
#define _INCLUDE_BASICEXIT_H_

/*
 * !!! these need to cleanup:
 * !!!  - Boxcars.
 * !!!  - OnEvent handlers.
 */

#define __exitSub   return

#define __exitFunction(returnValue)     return(returnValue)


#endif /* _INCLUDE_BASICEXIT_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* End of BasicExit.h ... */


