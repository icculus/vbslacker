/*
 * Test randomizing functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
#include "BasicLib.h"

extern long errors;
extern long warnings;



void test_rnd(void)
/*
 * Test RND(>= 0) functionality. Must be done once before any other
 *  RANDOMIZE or RND calls to succeed.
 *
 *    params : void.
 *   returns : void.
 */
{
    int i;
    __single rc;
    __single last;
    __single answers[] = {
                            0.7055475,
                            0.533424,
                            0.5795186,
                            0.2895625,
                            0.301948,
                            0.7747401,
                            0.01401764,
                            0.7607236,
                            0.81449,
                            0.7090379
                         };
    

    printf("Testing RND() and RND(0)...\n");

    for (i = 0; i < (sizeof (answers) / sizeof (__single)); i++)
    {
        rc = _vbf_rnd();
        if (rc != answers[i])
        {
            printf(" - Iteration #%d of RND returned (%f), instead of (%f).\n",
                    i, rc, answers[i]);
            errors++;
        } /* if */

        last = _vbff_rnd(0.0);
        if (last != rc)
        {
            printf(" - Iteration #%d of RND(0) returned (%f),"
                   "  when RND returned (%f).\n", i, last, rc);
            errors++;
        } /* if */
    } /* for */

} /* test_rnd */


void test_rnd_negative(void)
/*
 * Test RND(< 0) functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    int i;
    __single argVal = -10.0;
    __single rc;
    __single last;
    __single answers[] = {
                            0.3276443,
                            0.5151443,
                            0.7026443,
                            0.08832568,
                            0.4633257,
                            0.8383257,
                            0.2133257,
                            0.9633257,
                            0.7133257,
                            0.224007
                         };
    

    printf("Testing RND( < 0 )...\n");

    for (i = 0; i < (sizeof (answers) / sizeof (__single)); i++)
    {
        rc = _vbff_rnd(argVal);
        if (rc != answers[i])
        {
            printf(" - RND(%f) returned (%f), instead of (%f).\n",
                    argVal, rc, answers[i]);
            errors++;
        } /* if */

        last = _vbff_rnd(0.0);
        if (last != rc)
        {
            printf(" - RND(0) returned (%f), when RND(%f) returned (%f).\n",
                     last, argVal, rc);
            errors++;
        } /* if */

        argVal += 1.0;
    } /* for */
} /* test_rnd_negative */


void testRandomFunctions(void)
/*
 * Entry for tests for random functions, like RND and RANDOMIZE...
 *
 *      params : void.
 *     returns : void.
 */
{
    printf("\n[TESTING PSUEDORANDOM NUMBER GENERATION FUNCTIONS...]\n");

    test_rnd();
    test_rnd_negative();
//    test_randomize();
} /* testRandomFunctions */


#ifdef STANDALONE

long errors = 0;
long warnings = 0;

int main(int argc, char **argv)
{
    __initBasicLib(INITFLAG_DISABLE_CONSOLE, argc, argv);
    testRandomFunctions();
    return(0);
} /* main */

#endif

/* end of TestRandomFunctions.c ... */


