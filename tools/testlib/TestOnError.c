/*
 * Test OnError subsystem and some assembler stuff, too.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
#include "BasicLib.h"

#define TESTVAR_VALUE1 0xABCD
#define TESTVAR_VALUE2 0x1234
#define TESTVAR_VALUE3 "Look for this string in the data dump!"

#define RECURSION_COUNT 3

#define FAILED "  - Failed.\n"
#define FAILED_WITH_WARNING "  - Failed. Later tests may fail, too.\n"

extern long errors;
extern long warnings;

static int recursive = 0;
static void *_stack_ptr_;
static void *_base_ptr_;
static void *_stack_ptr2_;
static void *_base_ptr2_;


__boolean binaryDump(char *fileName, void *data, int size)
/*
 * Dump a chunk of memory to a binary file.
 *
 *   params : fileName == file to try to write to. It will destroy
 *                         any previous contents.
 *            data     == memory to write.
 *            size     == number of bytes to write, starting with (data).
 *  returns : __boolean true if data written, __boolean false otherwise.
 */
{
    FILE *binStream = fopen(fileName, "wb");
    __boolean retVal = false;

    if (binStream == NULL)
        printf("  - COULDN'T OPEN [%s] FOR BINARY OUTPUT!\n", fileName);
    else
    {
        if (fwrite(data, size, 1, binStream) != 1)
            printf("  - COULDN'T WRITE DATA DUMP TO DISK!\n");
        else
        {
            printf("  - Data dump is in [%s]...\n", fileName);
            retVal = true;
        } /* else */
        fclose(binStream);
    } /* else */

    return(retVal);
} /* binaryDump */


void test__getBasePointer(void)
/*
 * This function is pretty i386 specific. Your mileage may vary.
 *
 *  !!! more comments.
 */
{
    int var1 = TESTVAR_VALUE1;

    printf("Testing __getBasePointer()...\n");

    __getBasePointer(_base_ptr_);

    if (*((int *) (((int) _base_ptr_) - 4)) != TESTVAR_VALUE1)
    {
        printf(FAILED_WITH_WARNING);
        printf("  - dword [ebp] should have been (0x%X), but is (0x%X).\n",
                TESTVAR_VALUE1, *((int *) (((int) _base_ptr_) - 4)));
        errors++;
        if (binaryDump("debugBP.bin", _base_ptr_ - 100, 200) == true)
            printf("  - (That's our supposed [ebp-100] to [ebp+100]...)\n");
    } /* if */

    var1 = 0;  /* prevent compiler warnings... */
} /* test__getBasePointer */


void test__getStackPointer_recurse(void)
/*
 * This function calls itself RECURSION_COUNT times, to pile
 *  some data on the stack.
 */
{
    recursive++;
    if (recursive == RECURSION_COUNT)
        __getStackPointer(_stack_ptr_);
    else
        test__getStackPointer_recurse();
} /* test__getStackPointer_recurse */


void test__getStackPointer()
/*
 * The theory being that a void function with void arguments should
 *  add 8 bytes to the stack; four for the return address and four for the
 *  saved base pointer. So we grab the current stack pointer, and call
 *  a recursive function that'll grab the stack pointer before returning.
 *  By doing the math between the two pointers, we can see if we're
 *  grabbing it correctly.
 */
{
    void *expected;
    void *sp;

    printf("Testing __getStackPointer()...\n");

    __getStackPointer(sp);

    expected = (sp - (RECURSION_COUNT * 8));

    recursive = 0;
    test__getStackPointer_recurse();

    if (_stack_ptr_ != expected)
    {
        printf(FAILED_WITH_WARNING);
        printf("  - expected (%p), but got (%p)\n", expected, _stack_ptr_);
        warnings++;
    } /* if */
} /* test__getStackPointer */


void test__jump(void)
/*
 * This function is pretty i386 specific. Your mileage may vary.
 *
 *  !!! more comments.
 */
{
    void *addr;
    printf("Testing __jump()...\n");

    __getLabelAddr(jumpToLabel, addr);
    __jump(addr);
    printf("  - __jump() didn't jump.\n");
    errors++;

__insertLineLabel(jumpToLabel);
    return;
} /* test__jump */


void __triggerOnError_recurse(int recurse)
/*
 * This function calls itself RECURSION_COUNT times, to pile
 *  some data on the stack, then calls __runtimeError()...
 */
{
    if (recurse == RECURSION_COUNT)
        __runtimeError(ERR_DISK_NOT_READY);
    else
        __triggerOnError_recurse(recurse + 1);
} /* __triggerOnError_recurse */


void testOnErrorGotoHandling(int runCount)
/*
 * This tests ON ERROR GOTO functionality. We set up an error handler,
 *  build up a couple of function calls on the stack, and trigger an error.
 *
 * This code checks to see if the error handler is called correctly, and if
 *  the base pointer is correct. If the base pointer (and therefore the stack)
 *  is fucked up, we dump some debug information to disk, and terminate the
 *  program gracefully, before it is terminated ungracefully for us by a
 *  corrupted stack.
 *
 * This function should be called more than once (but should be called several
 *  times, if you err on the side of caution), to verify that
 *  __(de)registerOnErrorHandler() is working correctly. If not correctly, then
 *  correctly enough. testOnErrorGotoRecurseHandling() is a more rigorous
 *  test of Error handler (de)registration, since it stacks handlers.
 *
 *    params : runCount == count of times this function has been executed.
 *   returns : void.
 */
{
    __ONERRORVARS;
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
    char testVar3[] = TESTVAR_VALUE3;
    int landed = 0xEDFE;  /* looks like "FEED" in intel hexdump. */

    __ONERRORINIT;

    __getBasePointer(_base_ptr_);      /* set up globals for comparison. */
    __getStackPointer(_stack_ptr_);

    printf("Testing ON ERROR GOTO handling (run #%d)...\n", runCount);

    __setOnErrorHandler(errHandler1);
    __triggerOnError_recurse(0);
    __jumpLabel(missedHandler1);

__insertLineLabel(errHandler1);
    __getBasePointer(_base_ptr2_);     /* get 2nd globals for comparison. */
    __getStackPointer(_stack_ptr2_);

    if (_base_ptr2_ != _base_ptr_)
    {
        errors++;
        printf("  - Base pointer is damaged.\n");
        printf("  - EBP inside error handler SHOULD BE (%p)\n", _base_ptr_);
        printf("  - EBP inside error handler IS (%p)\n", _base_ptr2_);
    } /* if */

    if (_stack_ptr2_ != _stack_ptr_)
    {
        errors++;
        printf("  - Stack pointer is damaged.\n");
        printf("  - ESP inside error handler SHOULD BE (%p)\n", _stack_ptr_);
        printf("  - ESP inside error handler IS (%p)\n", _stack_ptr2_);
    } /* if */

    if ((_base_ptr2_ != _base_ptr_) || (_stack_ptr2_ != _stack_ptr_))
    {
        errors++;
        printf("  - testVar1 is (0x%X), should be (0x%X)...\n",
                                 testVar1, TESTVAR_VALUE1);
        printf("  - testVar2 is (0x%X), should be (0x%X)...\n",
                                 testVar2, TESTVAR_VALUE2);
        printf("  - testVar3 is [%s], should be legible...\n", testVar3);
        printf("  - landed is (0x%X)...\n", landed);
        if (binaryDump("debugErrGoto.bin", _base_ptr_ - 100, 200) == true)
            printf("  - (That's our supposed [ebp-100] to [ebp+100]...)\n");
        printf("  - Stack will be FUBAR. Must terminate testing.\n");
        exit(1);
    } /* if */

    landed = 1;
    __jumpLabel(endTest1);

__insertLineLabel(missedHandler1);
    landed = 2;
    __jumpLabel(endTest1);

__insertLineLabel(endTest1);
    switch (landed)
    {
        case 1:   /* good. */
            break;
        case 2:
            errors++;
            printf("  - Handler missed. Failed.\n"  
                   "  - Other tests will give incorrect results.\n"
                   "  - Must terminate testing.\n");
            exit(1);
            break;
        default:
            errors++;
            printf("  - Confused! Failed.\n"
                   "  - Other tests will give incorrect results.\n"
                   "  - Must terminate testing.\n");
            exit(1);
            break;
    } /* switch */

    testVar3[0] = '\0';  /* stops compiler whining. */

    __exitCleanupOnError;
} /* testOnErrorGotoHandling */


void testOnErrorGotoStackedHandling2(void)
/*
 * This tests recursive ON ERROR GOTO functionality. We set up an Error
 *  handler, trigger an onError, and inside the handler, trigger another
 *  onError.
 *
 * This function checks the same things testOnErrorsGotoHandling() does.
 * !!!! wrong comments...
 * This function should be called more than once (but should be called several
 *  times, if you err on the side of caution), to help verify that
 *  __(de)registerOnErrorHandler() is working correctly.
 *
 *    params : runCount == count of times this function has been executed.
 *   returns : void.
 */
{
    __ONERRORVARS;

    __ONERRORINIT;

    __setOnErrorHandler(errHandler2);
    __runtimeError(ERR_PERMISSION_DENIED);
    printf(FAILED);
    errors++;
    __jumpLabel(missedHandler2);

__insertLineLabel(errHandler2);
    __runtimeError(ERR_PATH_NOT_FOUND);

__insertLineLabel(missedHandler2);
    __exitCleanupOnError;
} /* testOnErrorGotoStackedHandling2 */


void testOnErrorGotoStackedHandling1(int runCount)
/*
 * This tests recursive ON [Error] GOTO functionality. We set up an Error
 *  handler, trigger an onError, and inside the handler, trigger another
 *  onError.
 *
 * This function checks the same things testOnErrorsGotoHandling() does.
 *
 * This function should be called more than once (but should be called several
 *  times, if you err on the side of caution), to help verify that
 *  __(de)registerOnErrorHandler() is working correctly.
 *
 *    params : runCount == count of times this function has been executed.
 *   returns : void.
 */
{
    __ONERRORVARS;
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
    char testVar3[] = TESTVAR_VALUE3;
    int landed = 0xEDFE;  /* looks like "FEED" in intel hexdump. */

    __ONERRORINIT;

    __getBasePointer(_base_ptr_);      /* set up globals for comparison. */
    __getStackPointer(_stack_ptr_);

    printf("Testing stacked ON ERROR GOTO handling (run #%d)...\n", runCount);

    __setOnErrorHandler(errHandler3);
    testOnErrorGotoStackedHandling2();
    __jumpLabel(missedHandler3);

__insertLineLabel(errHandler3);
    __getBasePointer(_base_ptr2_);     /* get 2nd globals for comparison. */
    __getStackPointer(_stack_ptr2_);

    if (_base_ptr2_ != _base_ptr_)
    {
        errors++;
        printf("  - Base pointer is damaged.\n");
        printf("  - EBP inside error handler SHOULD BE (%p)\n", _base_ptr_);
        printf("  - EBP inside error handler IS (%p)\n", _base_ptr2_);
    } /* if */

    if (_stack_ptr2_ != _stack_ptr_)
    {
        errors++;
        printf("  - Stack pointer is damaged.\n");
        printf("  - ESP inside error handler SHOULD BE (%p)\n", _stack_ptr_);
        printf("  - ESP inside error handler IS (%p)\n", _stack_ptr2_);
    } /* if */

    if ((_base_ptr2_ != _base_ptr_) || (_stack_ptr2_ != _stack_ptr_))
    {
        errors++;
        printf("  - testVar1 is (0x%X), should be (0x%X)...\n",
                                 testVar1, TESTVAR_VALUE1);
        printf("  - testVar2 is (0x%X), should be (0x%X)...\n",
                                 testVar2, TESTVAR_VALUE2);
        printf("  - testVar3 is [%s], should be legible...\n", testVar3);
        printf("  - landed is (0x%X)...\n", landed);
        if (binaryDump("debugErrGoto.bin", _base_ptr_ - 100, 200) == true)
            printf("  - (That's our supposed [ebp-100] to [ebp+100]...)\n");
        printf("  - Stack will be FUBAR. Must terminate testing.\n");
        exit(1);
    } /* if */

    landed = 1;
    __jumpLabel(endTest2);

__insertLineLabel(missedHandler3);
    landed = 2;
    __jumpLabel(endTest2);

__insertLineLabel(endTest2);
    switch (landed)
    {
        case 1:   /* good. */
            break;
        case 2:
            errors++;
            printf("  - Handler missed. Failed.\n"  
                   "  - Other tests will give incorrect results.\n"
                   "  - Must terminate testing.\n");
            exit(1);
            break;
        default:
            errors++;
            printf("  - Confused! Failed.\n"
                   "  - Other tests will give incorrect results.\n"
                   "  - Must terminate testing.\n");
            exit(1);
            break;
    } /* switch */

    testVar3[0] = '\0';  /* stops compiler whining. */

    __exitCleanupOnError;
} /* testOnErrorGotoStackedHandling1 */


void testResumeNext(int runCount)
/*
 * Test RESUME NEXT command. We trigger an Error, and see where control goes.
 *
 *  Run this multiple times, for the sake of the chaos factor.
 */
{
    __ONERRORVARS;
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
    char testVar3[] = TESTVAR_VALUE3;

    __ONERRORINIT;
    __getBasePointer(_base_ptr_);
    __getStackPointer(_stack_ptr_);
    __setInstructs(resumeZeroWrong, resumeNextRight);

    printf("Testing RESUME NEXT (run #%d)...\n", runCount);

    __setOnErrorHandler(resumeNextErrHandler);

    __runtimeError(ERR_BAD_FILE_MODE);
    __jumpLabel(resumeNextEnd);

__insertLineLabel(resumeNextErrHandler);
    __resumeNext;
    printf(FAILED);
    errors++;
    __jumpLabel(resumeNextEnd);

__insertLineLabel(resumeZeroWrong);
    printf("  - Resumed ZERO instead of NEXT.\n");
    errors++;
    __jumpLabel(resumeNextEnd);

__insertLineLabel(resumeNextRight);
    __getBasePointer(_base_ptr2_);
    __getStackPointer(_stack_ptr2_);

    if (_base_ptr2_ != _base_ptr_)
    {
        errors++;
        printf("  - Base pointer is damaged.\n");
        printf("  - EBP inside error handler SHOULD BE (%p)\n", _base_ptr_);
        printf("  - EBP inside error handler IS (%p)\n", _base_ptr2_);
    } /* if */

    if (_stack_ptr2_ != _stack_ptr_)
    {
        errors++;
        printf("  - Stack pointer is damaged.\n");
        printf("  - ESP inside error handler SHOULD BE (%p)\n", _stack_ptr_);
        printf("  - ESP inside error handler IS (%p)\n", _stack_ptr2_);
    } /* if */

    if ((_base_ptr2_ != _base_ptr_) || (_stack_ptr2_ != _stack_ptr_))
    {
        errors++;
        printf("  - testVar1 is (0x%X), should be (0x%X)...\n",
                                 testVar1, TESTVAR_VALUE1);
        printf("  - testVar2 is (0x%X), should be (0x%X)...\n",
                                 testVar2, TESTVAR_VALUE2);
        printf("  - testVar3 is [%s], should be legible...\n", testVar3);
        if (binaryDump("debugErrGoto.bin", _base_ptr_ - 100, 200) == true)
            printf("  - (That's our supposed [ebp-100] to [ebp+100]...)\n");
        printf("  - Stack will be FUBAR. Must terminate testing.\n");
        exit(1);
    } /* if */

__insertLineLabel(resumeNextEnd);
    __exitCleanupOnError;
} /* testResumeNext */


void testResumeZero(int runCount)
/*
 * Test RESUME 0 command. We trigger an Error, and see where control goes.
 *
 *  Run this multiple times, for the sake of the chaos factor.
 */
{
    __ONERRORVARS;
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
    char testVar3[] = TESTVAR_VALUE3;

    __ONERRORINIT;
    __getBasePointer(_base_ptr_);
    __getStackPointer(_stack_ptr_);
    __setInstructs(resumeZeroRight, resumeNextWrong);

    printf("Testing RESUME 0 (run #%d)...\n", runCount);

    __setOnErrorHandler(resumeZeroErrHandler);

    __runtimeError(ERR_RENAME_ACROSS_DISKS);
    __jumpLabel(resumeZeroEnd);

__insertLineLabel(resumeZeroErrHandler);
    __resumeZero;
    printf(FAILED);
    errors++;
    __jumpLabel(resumeZeroEnd);

__insertLineLabel(resumeZeroRight);
    __getBasePointer(_base_ptr2_);
    __getStackPointer(_stack_ptr2_);

    if (_base_ptr2_ != _base_ptr_)
    {
        errors++;
        printf("  - Base pointer is damaged.\n");
        printf("  - EBP inside error handler SHOULD BE (%p)\n", _base_ptr_);
        printf("  - EBP inside error handler IS (%p)\n", _base_ptr2_);
    } /* if */

    if (_stack_ptr2_ != _stack_ptr_)
    {
        errors++;
        printf("  - Stack pointer is damaged.\n");
        printf("  - ESP inside error handler SHOULD BE (%p)\n", _stack_ptr_);
        printf("  - ESP inside error handler IS (%p)\n", _stack_ptr2_);
    } /* if */

    if ((_base_ptr2_ != _base_ptr_) || (_stack_ptr2_ != _stack_ptr_))
    {
        errors++;
        printf("  - testVar1 is (0x%X), should be (0x%X)...\n",
                                 testVar1, TESTVAR_VALUE1);
        printf("  - testVar2 is (0x%X), should be (0x%X)...\n",
                                 testVar2, TESTVAR_VALUE2);
        printf("  - testVar3 is [%s], should be legible...\n", testVar3);
        if (binaryDump("debugErrGoto.bin", _base_ptr_ - 100, 200) == true)
            printf("  - (That's our supposed [ebp-100] to [ebp+100]...)\n");
        printf("  - Stack will be FUBAR. Must terminate testing.\n");
        exit(1);
    } /* if */
    __jumpLabel(resumeZeroEnd);

__insertLineLabel(resumeNextWrong);
    errors++;
    printf("  - Resumed NEXT instead of ZERO.\n");
    __jumpLabel(resumeZeroEnd);

__insertLineLabel(resumeZeroEnd);
    __exitCleanupOnError;
} /* testResumeNext */


void testResumeLabel(int runCount)
/*
 * Test RESUME linelabel command. We trigger an Error, and see where control
 *  goes.
 *
 *  Run this multiple times, for the sake of the chaos factor.
 */
{
    __ONERRORVARS;
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
    char testVar3[] = TESTVAR_VALUE3;

    __ONERRORINIT;
    __getBasePointer(_base_ptr_);
    __getStackPointer(_stack_ptr_);

    printf("Testing RESUME linelabel (run #%d)...\n", runCount);

    __setOnErrorHandler(resumeLabelErrHandler);

    __runtimeError(ERR_RENAME_ACROSS_DISKS);
    printf(FAILED);
    errors++;
    __jumpLabel(resumeLabelEnd);

__insertLineLabel(resumeLabelErrHandler);
    __resumeLabel(resumeLabel);
    printf(FAILED);
    errors++;
    __jumpLabel(resumeLabelEnd);

__insertLineLabel(resumeLabel);
    __getBasePointer(_base_ptr2_);
    __getStackPointer(_stack_ptr2_);

    if (_base_ptr2_ != _base_ptr_)
    {
        errors++;
        printf("  - Base pointer is damaged.\n");
        printf("  - EBP inside error handler SHOULD BE (%p)\n", _base_ptr_);
        printf("  - EBP inside error handler IS (%p)\n", _base_ptr2_);
    } /* if */

    if (_stack_ptr2_ != _stack_ptr_)
    {
        errors++;
        printf("  - Stack pointer is damaged.\n");
        printf("  - ESP inside error handler SHOULD BE (%p)\n", _stack_ptr_);
        printf("  - ESP inside error handler IS (%p)\n", _stack_ptr2_);
    } /* if */

    if ((_base_ptr2_ != _base_ptr_) || (_stack_ptr2_ != _stack_ptr_))
    {
        errors++;
        printf("  - testVar1 is (0x%X), should be (0x%X)...\n",
                                 testVar1, TESTVAR_VALUE1);
        printf("  - testVar2 is (0x%X), should be (0x%X)...\n",
                                 testVar2, TESTVAR_VALUE2);
        printf("  - testVar3 is [%s], should be legible...\n", testVar3);
        if (binaryDump("debugErrGoto.bin", _base_ptr_ - 100, 200) == true)
            printf("  - (That's our supposed [ebp-100] to [ebp+100]...)\n");
        printf("  - Stack will be FUBAR. Must terminate testing.\n");
        exit(1);
    } /* if */
    __jumpLabel(resumeLabelEnd);

__insertLineLabel(resumeLabelEnd);
    __exitCleanupOnError;
} /* testResumeLabel */


void testOnErrorState(void)
{
    if (__isOnErrorThreadStateNULL() != true)
    {
        warnings++;
        printf("  - OnError was not cleaned up correctly.\n"
               "  - Later tests will probably fail inexplicably.\n");
    } /* if */
} /* testOnErrorState */


void testOnErrorHandling(void)
/*
 * Test "on error" handling of various types.
 *
 *    params : void.
 *   returns : void.
 */
{
    int i;

    printf("\n[TESTING ON ERROR HANDLING...]\n");

    for (i = 1; i <= 3; i++)
    {
        testOnErrorGotoHandling(i);
        testOnErrorState();
    } /* for */

    for (i = 1; i <= 3; i++)
    {
        testOnErrorGotoStackedHandling1(i);
        testOnErrorState();
    } /* for */

    for (i = 1; i <= 3; i++)
    {
        testResumeNext(i);
        testOnErrorState();
    } /* for */

    for (i = 1; i <= 3; i++)
    {
        testResumeZero(i);
        testOnErrorState();
    } /* for */

    for (i = 1; i <= 3; i++)
    {
        testResumeLabel(i);
        testOnErrorState();
    } /* for */
} /* testOnErrorHandling */


void testOnError(void)
/*
 * Test ON Error functionality in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n[TESTING MACHINE-LEVEL MACROS...]\n");
    test__getBasePointer();
    test__getStackPointer();
    test__jump();
    testOnErrorHandling();
} /* testOnError */


#ifdef STANDALONE

long errors = 0;
long warnings = 0;

int main(int argc, char **argv, char **envp)
{
    __initBasicLib(INITFLAG_DISABLE_CONSOLE, argc, argv, envp);
    testOnError();
    return(0);
} /* main */

#endif

/* end of TestOnErrors.c ... */


