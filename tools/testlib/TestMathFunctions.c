/*
 * Test math functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <math.h>
#include "BasicLib.h"

void test_vbdd_abs(void)
/*
 * Test abs() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    double posVals[] = {999.092, 09284.2385, 845725.34328, 83.0, 0.0};
    double negVals[] = {-999.092, -09284.2385, -845725.34328, -83.0, 0.0};
    int i;
    double rc;

    printf("Testing ABS()...\n");

    for (i = 0; i < sizeof (posVals) / sizeof (double); i++)
    {
        rc = vbdd_abs(posVals[i]);
        if (rc != posVals[i])
            printf("  - abs(%f) returned (%f) incorrectly!\n", posVals[i], rc);

        rc = vbdd_abs(negVals[i]);
        if (rc != posVals[i])
            printf("  - abs(%f) returned (%f) incorrectly!\n", posVals[i], rc);
    } /* for */
} /* test_vbdd_abs */


void test_vbdd_sqr(void)
/*
 * Test sqr() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    __ONERRORVARS;
    double i;
    double rc;
    double correct;

    printf("Testing SQR()...\n");

    __ONERRORINIT;
    __setInstructs(trySqr, trySqrNext);

    __setOnErrorHandler(sqrHandler);

    for (i = -1000.0932; i < 99999.999; i += 213.42)
    {
        __runtimeError(ERR_NO_ERROR);
        correct = sqrt(i);
__insertLineLabel(trySqr);
        rc = vbdd_sqr(i);
__insertLineLabel(trySqrNext);
        if ((vbi_err() == ERR_NO_ERROR) && (rc != correct))
            printf("  - sqr(%f) returned (%f) incorrectly!\n", i, rc);
    } /* for */
    __exitCleanupOnError;
    return;

__insertLineLabel(sqrHandler);
    if (i >= 0)
        printf("  - sqr(%f) incorrectly threw an error!\n", i);
    __resumeNext;
} /* test_vbdd_sqr */



void test_vbdd_atn(void)
/*
 * Test atn() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    double i;
    double rc;
    double correct;

    printf("Testing ATN()...\n");

    for (i = -1000.0932; i < 99999.999; i += 213.42)
    {
        rc = vbdd_atn(i);
        correct = atan(i);
        if (rc != correct)
            printf("  - atn(%f) returned (%f) incorrectly!\n", i, rc);
    } /* for */
} /* test_vbdd_atn */


void test_vbdd_sin(void)
/*
 * Test sin() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    double i;
    double rc;
    double correct;

    printf("Testing SIN()...\n");

    for (i = -1000.0932; i < 99999.999; i += 213.42)
    {
        rc = vbdd_sin(i);
        correct = sin(i);
        if (rc != correct)
            printf("  - sin(%f) returned (%f) incorrectly!\n", i, rc);
    } /* for */
} /* test_vbdd_sin */


void test_vbdd_cos(void)
/*
 * Test cos() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    double i;
    double rc;
    double correct;

    printf("Testing COS()...\n");

    for (i = -1000.0932; i < 99999.999; i += 213.42)
    {
        rc = vbdd_cos(i);
        correct = cos(i);
        if (rc != correct)
            printf("  - cos(%f) returned (%f) incorrectly!\n", i, rc);
    } /* for */
} /* test_vbdd_cos */


void test_vbdd_tan(void)
/*
 * Test tan() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    double i;
    double rc;
    double correct;

    printf("Testing TAN()...\n");

    for (i = -1000.0932; i < 99999.999; i += 213.42)
    {
        rc = vbdd_tan(i);
        correct = tan(i);
        if (rc != correct)
            printf("  - tan(%f) returned (%f) incorrectly!\n", i, rc);
    } /* for */
} /* test_vbdd_tan */


#warning Figure out how the BASIC exp() works...

#if 0
void test_vbdd_exp(void)
/*
 * Test exp() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{ 
    double solutions[] = {  DOESNT WORK. INCORRECT.
                           0,
                           1,
                           pow(2.718282, 2),
                           pow(2.718282, 3),
                           pow(2.718282, 4),
                         };
    int i;
    double rc;

    printf("Testing EXP()...\n");

    for (i = 0; i < (sizeof (solutions) / sizeof (double)); i++)
    {
        rc = vbdd_exp(i);
        if (rc != solutions[i])
            printf("  - exp(%d) returned (%f) incorrectly!\n", i, rc);
    } /* for */
} /* test_vbdd_exp */
#endif


void test_vbdd_log(void)
/*
 * Test log() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    __ONERRORVARS;
    double i;
    double rc;
    double correct;

    printf("Testing LOG()...\n");

    __ONERRORINIT;

    __setInstructs(tryLog, tryLogNext);
    __setOnErrorHandler(logHandler);

    for (i = -1000.0932; i < 99999.999; i += 213.42)
    {
        __runtimeError(ERR_NO_ERROR);
        correct = log(i);
__insertLineLabel(tryLog);
        rc = vbdd_log(i);
__insertLineLabel(tryLogNext);
        if ((vbi_err() == ERR_NO_ERROR) && (rc != correct))
            printf("  - log(%f) returned (%f) incorrectly!\n", i, rc);
    } /* for */
    __exitCleanupOnError;
    return;

__insertLineLabel(logHandler);
    if (i >= 0)
        printf("  - log(%f) incorrectly threw an error!\n", i);
    __resumeNext;
} /* test_vbdd_log */


void test_vbld_fix(void)
/*
 * Test fix() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    double problems[]  = {0, -0.0000001, 999999.02, -12434.0, 643612.99999};
    double solutions[] = {0, 0, 999999, -12434, 643612};
    int i;
    double rc;

    printf("Testing FIX()...\n");

    for (i = 0; i < (sizeof (solutions) / sizeof (double)); i++)
    {
        rc = vbld_fix(problems[i]);
        if (rc != solutions[i])
            printf("  - fix(%d) returned (%f) incorrectly!\n", i, rc);
    } /* for */
} /* test_vbld_fix */


void test_vbld_int(void)
/*
 * Test int() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    double problems[]  = {0, -0.0000001, 999999.02, -12434.0, 643612.99999};
    double solutions[] = {0, -1, 999999, -12434, 643612};
    int i;
    double rc;

    printf("Testing INT()...\n");

    for (i = 0; i < (sizeof (solutions) / sizeof (double)); i++)
    {
        rc = vbld_int(problems[i]);
        if (rc != solutions[i])
            printf("  - int(%d) returned (%f) incorrectly!\n", i, rc);
    } /* for */
} /* test_vbld_int */


void test_vbid_sgn(void)
/*
 * Test sgn() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    double problems[]  = {0, -0.0000001, 999999.02, -12434.0, 643612.99999};
    double solutions[] = {0, -1, 1, -1, 1};
    int i;
    double rc;

    printf("Testing SGN()...\n");

    for (i = 0; i < (sizeof (solutions) / sizeof (double)); i++)
    {
        rc = vbid_sgn(problems[i]);
        if (rc != solutions[i])
            printf("  - sgn(%d) returned (%f) incorrectly!\n", i, rc);
    } /* for */
} /* test_vbid_sgn */


void testMathFunctions(void)
/*
 * This code tests all the conversion functions in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n[TESTING MATH FUNCTIONS...]\n");

    test_vbdd_abs();
    test_vbdd_sqr();
    test_vbdd_atn();
    test_vbdd_sin();
    test_vbdd_cos();
    test_vbdd_tan();
/*    test_vbdd_exp(); */
    test_vbdd_log();
    test_vbld_fix();
    test_vbld_int();
    test_vbid_sgn();
} /* testMathFunctions */


#ifdef STANDALONE

int main(int argc, char **argv)
{
    void *base;

    __getBasePointer(base);
    __initBasicLib(base, INITFLAG_DISABLE_CONSOLE, argc, argv);
    testMathFunctions();
    __deinitBasicLib();
    return(0);
} /* main */

#endif

/* end of TestMathFunctions.c ... */

