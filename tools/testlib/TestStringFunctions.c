/*
 * Test string functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
#include "BasicLib.h"


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


void cmpStr(STATEPARAMS, PBasicString pStr, char *data,
            int length, boolean fixedLength)
/*
 * This function checks a BasicString to verify that its various attributes
 *  are correct, and display details if they are not.
 *
 *    params : pStr == BasicString to check.
 *             data == what string's data field should hold.
 *             length == how long the string should be.
 *             fixedLength == should the string be fixed length?
 *   returns : void.
 */
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


void test___constString(STATEPARAMS)
/*
 * Test __constString functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString rc = NULL;

    printf("Testing __constString()...\n");

    rc = __constString(STATEARGS, TEST_TEXT2);
    cmpStr(STATEARGS, rc, TEST_TEXT2, strlen(TEST_TEXT2), true);

        /* extra test; not only must data match, but it must be same pointer. */
    if (rc != NULL)
    {
        if (rc->data != TEST_TEXT2)
            printf("  - New string's data is not constant.\n");
    } /* if */

    /* never __freeString() anything returned by __constString()! */
} /* test___constString */


void test___createString(STATEPARAMS)
/*
 * Test __createString functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString rc;

    printf("Testing __createString()...\n");

    rc = __createString(STATEARGS, TEST_TEXT1, true);
    cmpStr(STATEARGS, rc, TEST_TEXT1, strlen(TEST_TEXT1), true);
    __freeString(STATEARGS, rc);
} /* test___createString */


void test___assignString(STATEPARAMS)
/*
 * Test __assignString functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString copyThis = __createString(STATEARGS, TEST_TEXT1, true);
    PBasicString rc = NULL;

    printf("Testing __assignString()...\n");

    __assignString(STATEARGS, &rc, copyThis);
    cmpStr(STATEARGS, rc, copyThis->data, copyThis->length, false);
    __freeString(STATEARGS, rc);
    __freeString(STATEARGS, copyThis);
} /* test___assignString */


void test___catString(STATEPARAMS)
/*
 * Test __catString functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString str1 = __createString(STATEARGS, TEST_TEXT1, false);
    PBasicString str2 = __createString(STATEARGS, TEST_TEXT2, false);
    int length = str1->length + str2->length;
    char buffer[length + 1];

    printf("Testing __catString()...\n");

    sprintf(buffer, "%s%s", TEST_TEXT1, TEST_TEXT2);

    __catString(STATEARGS, &str1, str2);
    cmpStr(STATEARGS, str1, buffer, length, false);
    __freeString(STATEARGS, str1);
    __freeString(STATEARGS, str2);
} /* test___catString */


void test_len(STATEPARAMS)
/*
 * Test LEN() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(STATEARGS, TEST_TEXT1, false);
    int rc;

    printf("Testing LEN()...\n");

    rc = vbiS_len(STATEARGS, pStr);
    if (rc != pStr->length)
        printf("  - Failed. Returned (%d), expected (%d).\n", rc, pStr->length);

    __freeString(STATEARGS, pStr);
} /* test_len */


void test_space_DC_(STATEPARAMS)
/*
 * Test SPACE$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString rc;
    int length = strlen(TEST_SPACE);

    printf("Testing SPACE$()...\n");

    rc = vbSi_space_DC_(STATEARGS, length);
    cmpStr(STATEARGS, rc, TEST_SPACE, length, false);
    __freeString(STATEARGS, rc);
} /* test_space_DC_ */


void test_right_DC_(STATEPARAMS)
/*
 * Test RIGHT$() functionality. A string is right$()ed five times with
 *  different random sizes requested each time. Results are compared.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(STATEARGS, TEST_TEXT3, false);
    PBasicString rc;
    int length = pStr->length;
    int i;

    printf("Testing RIGHT$()...\n");

    for (i = 0; i <= length; i++)
    {
        rc = vbSSi_right_DC_(STATEARGS, pStr, i);
        cmpStr(STATEARGS, rc, (TEST_TEXT3 + length) - i, i, false);
        __freeString(STATEARGS, rc);
    } /* for */

    __freeString(STATEARGS, pStr);
} /* test_right_DC_ */


void test_left_DC_(STATEPARAMS)
/*
 * Test LEFT$() functionality. A string is left$()ed five times with
 *  different random sizes requested each time. Results are compared.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(STATEARGS, TEST_TEXT3, false);
    PBasicString rc;
    int length = pStr->length;
    int i;

    printf("Testing LEFT$()...\n");

    for (i = 1; i <= length; i++)
    {
        rc = vbSSi_left_DC_(STATEARGS, pStr, i);
        cmpStr(STATEARGS, rc, TEST_TEXT3, i, false);
        __freeString(STATEARGS, rc);
    } /* for */

    __freeString(STATEARGS, pStr);
} /* test_left_DC_ */


void test_ucase_DC_(STATEPARAMS)
/*
 * Test UCASE$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(STATEARGS, TEST_CASE, false);
    PBasicString rc;

    printf("Testing UCASE$()...\n");

    rc = vbSS_ucase_DC_(STATEARGS, pStr);
    cmpStr(STATEARGS, rc, TEST_UCASE, pStr->length, false);
    __freeString(STATEARGS, pStr);
    __freeString(STATEARGS, rc);
} /* test_ucase_DC_ */


void test_lcase_DC_(STATEPARAMS)
/*
 * Test LCASE$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(STATEARGS, TEST_CASE, false);
    PBasicString rc;

    printf("Testing LCASE$()...\n");

    rc = vbSS_lcase_DC_(STATEARGS, pStr);
    cmpStr(STATEARGS, rc, TEST_LCASE, pStr->length, false);
    __freeString(STATEARGS, pStr);
    __freeString(STATEARGS, rc);
} /* test_lcase_DC_ */


void test_ltrim_DC_(STATEPARAMS)
/*
 * Test LTRIM$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(STATEARGS, TEST_TRIM, false);
    PBasicString rc;

    printf("Testing LTRIM$()...\n");

    rc = vbSS_ltrim_DC_(STATEARGS, pStr);
    cmpStr(STATEARGS, rc, TEST_LTRIM, strlen(TEST_LTRIM), false);
    __freeString(STATEARGS, pStr);
    __freeString(STATEARGS, rc);
} /* test_ltrim_DC_ */


void test_rtrim_DC_(STATEPARAMS)
/*
 * Test RTRIM$() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    PBasicString pStr = __createString(STATEARGS, TEST_TRIM, false);
    PBasicString rc;

    printf("Testing RTRIM$()...\n");

    rc = vbSS_rtrim_DC_(STATEARGS, pStr);
    cmpStr(STATEARGS, rc, TEST_RTRIM, strlen(TEST_RTRIM), false);
    __freeString(STATEARGS, pStr);
    __freeString(STATEARGS, rc);
} /* test_rtrim_DC_ */


void testInternalStringFuncs(STATEPARAMS)
/*
 * Entry for tests for internal string functions, like __createString() and
 *  __assignString()...
 *
 *      params : void.
 *     returns : void.
 */
{
    printf("\n[TESTING INTERNAL STRING FUNCTIONS...]\n");

    test___constString(STATEARGS);
    test___createString(STATEARGS);
    test___assignString(STATEARGS);
    test___catString(STATEARGS);
} /* testInternalStringFuncs */



void testUserStringFuncs(STATEPARAMS)
/*
 * Entry for tests for user-called string functions, like rtrim$() and
 *  left$()...
 *
 *      params : void.
 *     returns : void.
 */
{
    printf("\n[TESTING USER-CALLED STRING FUNCTIONS...]\n");

    test_len(STATEARGS);
    test_space_DC_(STATEARGS);
    test_right_DC_(STATEARGS);
    test_left_DC_(STATEARGS);
    test_rtrim_DC_(STATEARGS);
    test_ltrim_DC_(STATEARGS);
    test_lcase_DC_(STATEARGS);
    test_ucase_DC_(STATEARGS);
} /* testUserStringFuncs */



void testStringFunctions(STATEPARAMS)
/*
 * This code tests all the string functions in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    testInternalStringFuncs(STATEARGS);
    testUserStringFuncs(STATEARGS);
} /* testStringFunctions */


#ifdef STANDALONE

int main(void)
{
    __initBasicLib(NULLSTATEARGS, INITFLAG_DISABLE_CONSOLE);
    testStringFunctions(NULLSTATEARGS);
    __deinitBasicLib();
    return(0);
} /* main */

#endif

/* !!! check to see if all these are tested...
PBasicString vbSSi_right_DC_(STATEPARAMS, PBasicString pStr, int count);
PBasicString vbSSi_left_DC_(STATEPARAMS, PBasicString pStr, int count);
PBasicString vbSS_rtrim_DC_(STATEPARAMS, PBasicString pStr);
PBasicString vbSS_ltrim_DC_(STATEPARAMS, PBasicString pStr);
PBasicString vbSS_lcase_DC_(STATEPARAMS, PBasicString pStr);
PBasicString vbSS_ucase_DC_(STATEPARAMS, PBasicString pStr);
PBasicString vbSi_space_DC_(STATEPARAMS, int length);
PBasicString vbSii_string_DC_(STATEPARAMS, int rep, int ch);
PBasicString vbSiS_string_DC_(STATEPARAMS, int rep, PBasicString strCh);
int vbiiSS_instr(STATEPARAMS, int start, PBasicString str1, PBasicString str2);
int vbiSS_instr(STATEPARAMS, PBasicString str1, PBasicString str2);
int vbiS_len(STATEPARAMS, PBasicString pStr);
*/


/* end of TestStringFunctions.c ... */


