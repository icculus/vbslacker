/*
 * Test conversion functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include "Initialize.h"
#include "OnEvents.h"
#include "ConversionFunctions.h"
#include "BasicString.h"
#include "ErrorFunctions.h"


void test_chr_DC_(int x) /* !!! */
/*
 * Test chr$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    int i;
    PBasicString rc;

    printf("Testing chr_DC_()...\n");

    __obtainThreadLock(&registerLock);
    __getStackPointer(&_stack_ptr_);
    __getBasePointer(&_base_ptr_);
    __registerOnEventHandler(&&chrError, &x + sizeof (x),
                             _stack_ptr_, _base_ptr_, ONERROR);
    __releaseThreadLock(&registerLock);


    for (i = -32767; i <= 32767; i++)
    {
        __basicErrno = ERR_NO_ERROR;
        rc = chr_DC_(i);
        if (__basicErrno == ERR_NO_ERROR)
        {
            if (rc->length != 1)
            {
                printf("  - chr$(%d) returned incorrect string length [%d]!\n",
                   (int) i, rc->length);
            } /* if */
            else if (rc->data[0] != (unsigned char) i)
                printf("  - chr$(%d) returned '\\%d'!\n", i, (int) rc->data[0]);
            __freeString(rc);
        } /* if */
    } /* for */

    __deregisterOnEventHandler(&x + sizeof (x), ONERROR);
    return;

chrError:              /* error handler... */ 
    __markOnEventHandlerAddr;

    if ((i >= 0) && (i <= 255))  /* !!! string of error number? */
        printf("  - chr$(%d) threw error %d!\n", i, __basicErrno);

    __resumeNext();
} /* test_chr_DC_ */


void test_str_DC_(void)
/*
 * Test str$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    double i;
    PBasicString rc;
    char buffer[40];

    printf("Testing str_DC_() (simple test)...\n");

    i = -32523.9921;
    sprintf(buffer, "%s%f", (i < 0.0) ? "" : " ", i);
    rc = str_DC_(i);

    if (rc->length > sizeof (buffer))
        printf("  - str$(%f) returned %d byte string.\n", i, rc->length);

    else if (memcmp(rc->data, buffer, rc->length) != 0)
    {
        memcpy(buffer, rc->data, rc->length);
        buffer[rc->length] = '\0';
        printf("  - str$(%f) returned \"%s\"!\n", i, buffer);
    } /* if */

    __freeString(rc);
} /* test_str_DC_ */


void test_asc(int x)  /* !!! */
/*
 * Test asc() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    int i;
    PBasicString argStr = __createString("The Quick brown fox... ", false);
    unsigned int rc;

    printf("Testing asc()...\n");

    for (i = 0; i < 255; i++)
    {
        argStr->data[0] = i;
        rc = asc(argStr);
        if (rc != i)
        {
            argStr->data[argStr->length - 1] = '\0';
            printf("  - asc(\"%s\") returned [%d]!\n", argStr->data, rc);
        } /* if */
    } /* for */

    __freeString(argStr);
} /* test_asc */


void test_hex_DC_(void)
/*
 * Test hex$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    int i;
    PBasicString rc;
    char buffer[30];

    printf("Testing hex$()...\n");

    for (i = 0; i < 0xFFFF; i++)
    {
        sprintf(buffer, "%X", i);
        rc = hex_DC_(i);

        if (memcmp(rc->data, buffer, rc->length) != 0)
        {
            memcpy(buffer, rc->data, rc->length);
            buffer[rc->length] = '\0';
            printf("  - hex$(0x%X) returned \"%s\"!\n", i, buffer);
        } /* if */

        __freeString(rc);
    } /* for */
} /* test_hex_DC_ */


void test_oct_DC_(void)
/*
 * Test oct$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    int i;
    PBasicString rc;
    char buffer[30];

    printf("Testing oct$()...\n");

    for (i = 0; i < 0xFFFF; i++)
    {
        sprintf(buffer, "%o", i);
        rc = oct_DC_(i);

        if (memcmp(rc->data, buffer, rc->length) != 0)
        {
            memcpy(buffer, rc->data, rc->length);
            buffer[rc->length] = '\0';
            printf("  - hex$(%d) returned \"%s\"!\n", i, buffer);
        } /* if */

        __freeString(rc);
    } /* for */
} /* test_oct_DC_ */


void test_val(void)
{
    double i;
    double rc;
    PBasicString pStr;
    char buffer[30];
    int j;
    int x = 0;

    printf("Testing val() (simple test)...\n");

    for (i = -999999.999; i < 999999.999; i += 1.111)
    {
        sprintf(buffer, " \t%f", i);

        x++;
        if (x == 5)   /* cram a space into number... */
        {
            for (j = sizeof (buffer) - 2; j >= 5; j--)
                buffer[j + 1] = buffer[j];
            buffer[5] = ' ';
        } /* if */

        pStr = __createString(buffer, false);

        rc = val(pStr);
        if (rc != i)
            printf("  - val(\"%s\") returned [%f]!\n", buffer, rc);

        __freeString(pStr);
    } /* for */

    /* !!! test for nulls/non numeric strings... */
} /* test_val */


void test_str_DC_and_val(void)
/*
 * Does str$ and val() testing by passing a value between both and
 *  seeing if it comes back to the original value.
 *
 *     params : void.
 *    returns : void.
 */
{
    double i;
    double rcD;
    PBasicString rcS;
    
    printf("Testing str$() and val() (complex test)...\n");

    for (i = -2000.9274; i < 32523.9921; i += 321.3214);
    {
        rcS = str_DC_(i);
        rcD = val(rcS);
        __freeString(rcS);
        if (rcD != i)
            printf("  - Value (%f) came back as (%f).\n", i, rcD);
    } /* for */
} /* test_str_DC_and_val */


void testConversionFunctions(void)
/*
 * This code tests all the conversion functions in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n[TESTING CONVERSION FUNCTIONS...]\n");

    test_chr_DC_(1);
    test_asc(1);
    test_str_DC_();
    test_str_DC_and_val();
/*    test_val();  !!! */
    test_hex_DC_();
    test_oct_DC_();
} /* testConversionFunctions */


#ifdef STANDALONE

int main(void)
{
    __initBasicLib();
    testConversionFunctions();
    __deinitBasicLib();
} /* main */

#endif

/* end of TestConversionFunctions.c ... */


