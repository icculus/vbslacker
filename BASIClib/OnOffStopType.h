/*
 * This type is for functions, like PEN and TIMER, that expect a
 *  constant of ON, OFF, or STOP. It is signified in the name
 *  mangling as 'O', for "OnOffStopType."
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_ONOFFSTOPTYPE_H_
#define _INCLUDE_ONOFFSTOPTYPE_H_

typedef enum { ON, OFF, STOP } OnOffStopType;

#endif /* _INCLUDE_ONOFFSTOPTYPE_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of OnOffStopType.h ... */

