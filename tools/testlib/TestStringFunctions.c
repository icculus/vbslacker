/*
 * Test string functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
#include "BasicString.h"
#include "StringFunctions.h"
#include "Boolean.h"

typedef unsigned char uchar;
uchar TEST_TEXT1[] = "This is a test string for TestStringFunctions.c!\n";
uchar TEST_TEXT2[] = "Here's another string for testing!\n";
uchar TEST_TEXT3[] = "String for testing RIGHT$, LEFT$, MID$, etc$...";
uchar TEST_SPACE[] = "             ";
uchar TEST_CASE[]  = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123";
uchar TEST_UCASE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ123";
uchar TEST_LCASE[] = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz123";
uchar TEST_TRIM[]  = "        Here's some trimmable spaces!     ";
uchar TEST_LTRIM[] =         "Here's some trimmable spaces!     ";
uchar TEST_RTRIM[] = "        Here's some trimmable spaces!";


void cmpStr(PBasicString pStr, char *data, int length, boolean fixedLength)
/* !!! comment this. */
{
    if (pStr == NULL)
        printf("  - New string is NULL!\n");
    else
    {
        if (pStr->length != length)
        {
            printf("  - New string's length is (%d), should be (%d).\n",
                   pStr->length, length);
        } /* if */

        if (pStr->fixedLength != fixedLength)
            printf("  - New string's fixedLength flag is incorrect.\n");

        if ((pStr->data == NULL) && (pStr->length != 0))
            printf("  - New string's data field is NULL.\n");
        else if (memcmp(pStr->data, data, length) != 0)
            printf("  - New string's data does not match original!\n");
    } /* else */
} /* cmpStr */


void test___constString(void)
/*
 * Test __constString functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString rc = NULL;

    printf("Testing __constString()...\n");

    rc = __constString(TEST_TEXT2);
    cmpStr(rc, TEST_TEXT2, strlen(TEST_TEXT2), true);

        /* extra test; not only must data match, but it must be same pointer. */
    if (rc != NULL)
    {
        if (rc->data != TEST_TEXT2)
            printf("  - New string's data is not constant.\n");
    } /* if */

    /* never __freeString() anything returned by __constString()! */
} /* test___constString */


void test___createString(void)
/*
 * Test __createString functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString rc;

    printf("Testing __createString()...\n");

    rc = __createString(TEST_TEXT1, true);
    cmpStr(rc, TEST_TEXT1, strlen(TEST_TEXT1), true);
    __freeString(rc);
} /* test___createString */


void test___assignString(void)
/*
 * Test __assignString functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString copyThis = __createString(TEST_TEXT1, true);
    PBasicString rc = NULL;

    printf("Testing __assignString()...\n");

    __assignString(&rc, copyThis);
    cmpStr(rc, copyThis->data, copyThis->length, false);
    __freeString(rc);
    __freeString(copyThis);
} /* test___assignString */


void test___catString(void)
/*
 * Test __catString functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString str1 = __createString(TEST_TEXT1, false);
    PBasicString str2 = __createString(TEST_TEXT2, false);
    int length = str1->length + str2->length;
    char buffer[length + 1];

    printf("Testing __catString()...\n");

    sprintf(buffer, "%s%s", TEST_TEXT1, TEST_TEXT2);

    __catString(&str1, str2);
    cmpStr(str1, buffer, length, false);
    __freeString(str1);
    __freeString(str2);
} /* test___catString */


void testInternalStringFuncs(void)
/*
 * Entry for tests for internal string functions, like __createString() and
 *  __assignString()...
 *
 *      params : void.
 *     returns : void.
 */
{
    printf("\n[TESTING INTERNAL STRING FUNCTIONS...]\n");

    test___constString();
    test___createString();
    test___assignString();
    test___catString();
} /* testInternalStringFuncs */


void test_len(void)
/*
 * Test LEN() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(TEST_TEXT1, false);
    int rc;

    printf("Testing len()...\n");

    rc = len(pStr);
    if (rc != pStr->length)
        printf("  - Failed. Returned (%d), expected (%d).\n", rc, pStr->length);

    __freeString(pStr);
} /* test_len */


void test_space_DC_(void)
/*
 * Test SPACE$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString rc;
    int length = strlen(TEST_SPACE);

    printf("Testing space_DC_()...\n");

    rc = space_DC_(length);
    cmpStr(rc, TEST_SPACE, length, false);
    __freeString(rc);
} /* test_space_DC_ */


void test_right_DC_(void)
/*
 * Test RIGHT$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(TEST_TEXT3, false);
    PBasicString rc;
    int length = pStr->length;
    int i;

    printf("Testing right_DC_()...\n");

    for (i = 0; i <= length; i++)
    {
        rc = right_DC_(pStr, i);
        cmpStr(rc, (TEST_TEXT3 + length) - i, i, false);
        __freeString(rc);
    } /* for */

    __freeString(pStr);
} /* test_right_DC_ */


void test_left_DC_(void)
/*
 * Test LEFT$() functionality. A string is left$()ed five times with
 *  different random sizes requested each time. Results are compared.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(TEST_TEXT3, false);
    PBasicString rc;
    int length = pStr->length;
    int i;

    printf("Testing left_DC_()...\n");

    for (i = 1; i <= length; i++)
    {
        rc = left_DC_(pStr, i);
        cmpStr(rc, TEST_TEXT3, i, false);
        __freeString(rc);
    } /* for */

    __freeString(pStr);
} /* test_left_DC_ */


void test_ucase_DC_(void)
/*
 * Test UCASE$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(TEST_CASE, false);
    PBasicString rc;

    printf("Testing ucase_DC_()...\n");

    rc = ucase_DC_(pStr);
    cmpStr(rc, TEST_UCASE, pStr->length, false);
    __freeString(pStr);
    __freeString(rc);
} /* test_ucase_DC_ */


void test_lcase_DC_(void)
/*
 * Test LCASE$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(TEST_CASE, false);
    PBasicString rc;

    printf("Testing lcase_DC_()...\n");

    rc = lcase_DC_(pStr);
    cmpStr(rc, TEST_LCASE, pStr->length, false);
    __freeString(pStr);
    __freeString(rc);
} /* test_lcase_DC_ */


void test_ltrim_DC_(void)
/*
 * Test LTRIM$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(TEST_TRIM, false);
    PBasicString rc;

    printf("Testing ltrim_DC_()...\n");

    rc = ltrim_DC_(pStr);
    cmpStr(rc, TEST_LTRIM, strlen(TEST_LTRIM), false);
    __freeString(pStr);
    __freeString(rc);
} /* test_ltrim_DC_ */


void test_rtrim_DC_(void)
/*
 * Test RTRIM$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(TEST_TRIM, false);
    PBasicString rc;

    printf("Testing rtrim_DC_()...\n");

    rc = rtrim_DC_(pStr);
    cmpStr(rc, TEST_RTRIM, strlen(TEST_RTRIM), false);
    __freeString(pStr);
    __freeString(rc);
} /* test_rtrim_DC_ */


void testUserStringFuncs(void)
/*
 * Entry for tests for user-called string functions, like rtrim$() and
 *  left$()...
 *
 *      params : void.
 *     returns : void.
 */
{
    printf("\n[TESTING USER-CALLED STRING FUNCTIONS...]\n");

    test_len();
    test_space_DC_();
    test_right_DC_();
    test_left_DC_();
    test_rtrim_DC_();
    test_ltrim_DC_();
    test_lcase_DC_();
    test_ucase_DC_();

    /* !!! need mid$ tests! */
} /* testUserStringFuncs */



void testStringFunctions(void)
/*
 * This code tests all the string functions in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    testInternalStringFuncs();
    testUserStringFuncs();
} /* testStringFunctions */


#ifdef STANDALONE

#include "Initialize.h"

int main(void)
{
    __initBasicLib();
    testStringFunctions();
    __deinitBasicLib();
} /* main */

#endif

/* end of TestStringFunctions.c ... */


