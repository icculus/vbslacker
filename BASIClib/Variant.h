/*
 * Declarations for VARIANT data type support.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_VARIANT_H_
#define _INCLUDE_VARIANT_H_

#ifdef __cplusplus
extern "C" {
#endif

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
        __integer _integer;
        __long _long;
        __single _single;
        __double _double;
        PBasicString _string;
        __boolean _boolean;
    } data;
} Variant;

typedef Variant *PVariant;

/* !!! need __intrinsicToVariant() calls, too. */
void __freeVariant(PVariant var);
int __variantToInt(PVariant var);
long __variantToLong(PVariant var);
float __variantToFloat(PVariant var);
double __variantToDouble(PVariant var);
PBasicString __variantToString(PVariant pVar, __boolean byRef);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_VARIANT_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of Variant.h ... */


