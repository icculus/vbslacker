/*
 * Declarations for VARIANT data type support.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_VARIANT_H_
#define _INCLUDE_VARIANT_H_

/*
 * Variant data types, for Variant.type field ...
 */

typedef enum {
                INTEGER,
                LONG,
                SINGLE,
                DOUBLE,
                STRING,
                BOOLEAN,
             } VariantType;


/*
 * Variant variable structure, for BASIC to C abstraction...
 */

typedef struct
{
    VariantType type;
    union
    {
        int _integer;
        long _long;
        float _single;
        double _double;
        PBasicString _string;
        boolean _boolean;
    } data;
} Variant;

typedef Variant *PVariant;

/* !!! need __intrinsicToVariant() calls, too. */
void __freeVariant(STATEPARAMS, PVariant var);
int __variantToInt(STATEPARAMS, PVariant var);
long __variantToLong(STATEPARAMS, PVariant var);
float __variantToFloat(STATEPARAMS, PVariant var);
double __variantToDouble(STATEPARAMS, PVariant var);
PBasicString __variantToString(STATEPARAMS, PVariant pVar, boolean byRef);

#endif
#endif

/* end of Variant.h ... */


