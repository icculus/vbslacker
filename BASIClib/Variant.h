/*
 * Declarations for VARIANT data type support.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */


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

#endif

/* end of Variant.h ... */


