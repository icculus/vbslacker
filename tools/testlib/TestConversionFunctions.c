/*
 * Test conversion functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include "BasicLib.h"

int __basicErrno;

void test_chr_DC_(void)
/*
 * Test chr$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    __ONERRORVARS;
    int i;
    PBasicString rc;

    printf("Testing CHR$()...\n");

    __ONERRORINIT;

    __setOnErrorHandler(chrError);
    __setInstructs(chrErrorResume, chrErrorResumeNext);

    for (i = -32767; i <= 32767; i++)
    {
        __basicErrno = ERR_NO_ERROR;

__insertLineLabel(chrErrorResume);

        rc = vbSi_chr_DC_(i);

__insertLineLabel(chrErrorResumeNext);

        if (__basicErrno == ERR_NO_ERROR)
        {
            if (rc->length != 1)
            {
                printf("  - CHR$(%d) returned incorrect string length [%d]!\n",
                   (int) i, rc->length);
            } /* if */
            else if (rc->data[0] != (unsigned char) i)
                printf("  - CHR$(%d) returned '\\%d'!\n", i, (int) rc->data[0]);
            __freeString(rc);
        } /* if */
    } /* for */

    __exitCleanupOnError;
    return;

__insertLineLabel(chrError);   /* error handler... */
    if ((i >= 0) && (i <= 255))  /* !!! string of error number? */
        printf("  - CHR$(%d) threw error %d!\n", i, __basicErrno);

    __resumeNext;
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

    printf("Testing STR$() (simple test)...\n");

    i = -32523.9921;
    sprintf(buffer, "%s%f", (i < 0.0) ? "" : " ", i);
    rc = vbSd_str_DC_(i);

    if (rc->length > sizeof (buffer))
        printf("  - STR$(%f) returned %d byte string.\n", i, rc->length);

    else if (memcmp(rc->data, buffer, rc->length) != 0)
    {
        memcpy(buffer, rc->data, rc->length);
        buffer[rc->length] = '\0';
        printf("  - STR$(%f) returned \"%s\"!\n", i, buffer);
    } /* if */

    __freeString(rc);
} /* test_str_DC_ */


void test_asc(void)
/*
 * Test asc() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    __ONERRORVARS;
    int i;
    int rc;
    PBasicString argStr = __createString("The Quick brown fox... ",
                                         false);

    __ONERRORINIT;

    __setInstructs(ascErrorResume, ascErrorResumeNext);

    printf("Testing ASC()...\n");

    __setOnErrorHandler(ascError);

    for (i = -32767; i < 32767; i++)
    {
        argStr->data[0] = i;
        __basicErrno = ERR_NO_ERROR;

__insertLineLabel(ascErrorResume);
        rc = vbiS_asc(argStr);

__insertLineLabel(ascErrorResumeNext);
        if ((vbi_err() == ERR_NO_ERROR) && (rc != i))
        {
            argStr->data[argStr->length - 1] = '\0';
            printf("  - ASC(\"%s\") returned [%d]! Should be (%d).\n",
                     argStr->data, rc, i);
        } /* if */
    } /* for */

    __freeString(argStr);
    __exitCleanupOnError;
    return;

__insertLineLabel(ascError);
    argStr->data[argStr->length - 1] = '\0';
    if ((i >= 0) && (i <= 255))
    {
        printf("  - ASC(\"%s\") gave error for valid argument!\n",
                argStr->data);
    } /* if */

    __resumeNext;
} /* test_asc */


void test_hex_DC_(void)
/*
 * Test hex$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    unsigned int i;
    PBasicString rc;
    char buffer[30];

    printf("Testing HEX$()...\n");

    for (i = 0; i < 0xFFFF; i++)
    {
        sprintf(buffer, "%X", i);
        rc = vbSl_hex_DC_(i);

        if (memcmp(rc->data, buffer, rc->length) != 0)
        {
            memcpy(buffer, rc->data, rc->length);
            buffer[rc->length] = '\0';
            printf("  - HEX$(&H%X) returned \"%s\"!\n", i, buffer);
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

    printf("Testing OCT$()...\n");

    for (i = 0; i < 0xFFFF; i++)
    {
        sprintf(buffer, "%o", i);
        rc = vbSl_oct_DC_(i);

        if (memcmp(rc->data, buffer, rc->length) != 0)
        {
            memcpy(buffer, rc->data, rc->length);
            buffer[rc->length] = '\0';
            printf("  - OCT$(%d) returned \"%s\"!\n", i, buffer);
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

    printf("Testing VAL() (simple test)...\n");

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

        rc = vbdS_val(pStr);
        if (rc != i)
            printf("  - VAL(\"%s\") returned [%f]!\n", buffer, rc);

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
    
    printf("Testing STR$() and VAL() (complex test)...\n");

    for (i = -2000.9274; i < 32523.9921; i += 321.3214);
    {
        rcS = vbSd_str_DC_(i);
        rcD = vbdS_val(rcS);
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

    test_chr_DC_();
    test_asc();
    test_str_DC_();
    test_str_DC_and_val();
#warning test_val() is commented out! Fix val()!!!
/*    test_val();  !!! */
    test_hex_DC_();
    test_oct_DC_();
} /* testConversionFunctions */


#ifdef STANDALONE

int main(void)
{
    __initBasicLib(INITFLAG_DISABLE_CONSOLE);
    testConversionFunctions();
    __deinitBasicLib();
    return(0);
} /* main */

#endif

/* end of TestConversionFunctions.c ... */


