/*
 * Test conversion functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include "ConversionFunctions.h"
#include "BasicString.h"


void test_chr_DC_(void)
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

    /* !!! ON ERROR RESUME NEXT */

    for (i = 0; i <= 255; i++)
    {
        rc = chr_DC_(i);
        if (rc->length != 1)
        {
            printf("  - chr$(%d) returned incorrect string length [%d]!\n",
                   (int) i, rc->length);
        } /* if */
        else if ((unsigned char) rc->data[0] != (unsigned char) i)
            printf("  - chr$(%d) returned '\\%d'!\n", i, (int) rc->data[0]);
        __freeString(rc);
    } /* for */

    /* !!! add test for out of range data when on error support is in. */
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

    printf("Testing str_DC_()...\n");

    for (i = -2000.58; i < 32523.9921; i += 33.912)
    {
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
    } /* for */
} /* test_str_DC_ */


void test_asc(void)
/*
 * Test asc() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    unsigned int i;
    PBasicString argStr = __createString("The Quick brown fox...\r\n", false);
    unsigned int rc;

    /* ON ERROR RESUME NEXT */

    printf("Testing asc()...\n");

    for (i = 0; i < 255; i++)
    {
        argStr->data[0] = i;
        rc = asc(argStr);

        if (rc != i)
            printf("  - asc(chr$(%u)) returned [%u]!\n", i, rc);
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

    printf("Testing val()...\n");

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


void testConversions(void)
/*
 * This code tests all the conversion functions in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n[TESTING CONVERSION FUNCTIONS...]\n");

    test_chr_DC_();
    test_str_DC_();
    test_asc();
/*    test_val();  !!! */
    test_hex_DC_();
    test_oct_DC_();
} /* testConversions */


#ifdef STANDALONE

int main(void)
{
    __initBasicLib();
    testConversions();
    __deinitBasicLib();
} /* main */

#endif

/* end of TestConversionFunctions.c ... */


