/*
 * Test conversion functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
#include "Threads.h"
#include "OnEvents.h"
#include "ErrorFunctions.h"
#include "Boolean.h"
#include "RegState.h"

#define TESTVAR_VALUE1 0xABCD
#define TESTVAR_VALUE2 0x1234
#define TESTVAR_VALUE3 "Look for this string in the data dump!"

#define RECURSION_COUNT 3

#define FAILED "  - Failed.\n"
#define FAILED_WITH_WARNING "  - Failed. Later tests will fail, too.\n"

static int recursive = 0;
static void *_stack_ptr_;
static void *_base_ptr_;
static void *_stack_ptr2_;
static void *_base_ptr2_;


boolean binaryDump(char *fileName, void *data, int size)
/*
 * Dump a chunk of memory to a binary file.
 *
 *   params : fileName == file to try to write to. It will destroy
 *                         any previous contents.
 *            data     == memory to write.
 *            size     == number of bytes to write, starting with (data).
 *  returns : boolean true if data written, boolean false otherwise.
 */
{
    FILE *binStream = fopen(fileName, "wb");
    boolean retVal = false;

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

    __getBasePointer(&_base_ptr_);

    if (*((int *) (((int) _base_ptr_) - 4)) != TESTVAR_VALUE1)
    {
        printf(FAILED_WITH_WARNING);
        printf("  - dword [ebp] should have been (0x%X), but is (0x%X).\n",
                TESTVAR_VALUE1, *((int *) (((int) _base_ptr_) - 4)));
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
        __getStackPointer(&_stack_ptr_);
    else
        test__getStackPointer_recurse();
} /* test__getStackPointer_recurse */


void __triggerOnEvent_recurse(STATEPARAMS, OnEventTypeEnum evType,
                              boolean printErr)
/*
 * This function calls itself RECURSION_COUNT times, to pile
 *  some data on the stack, then calls __triggerOnEvent(evType)...
 */
{
    recursive++;
    if (recursive == RECURSION_COUNT)
        __triggerOnEventByType(STATEARGS, evType);
    else
        __triggerOnEvent_recurse(STATEARGS, evType, printErr);

    if (printErr)   /* if we fell here without a RESUME NEXT... */
        printf("  - Event handler returns incorrectly.\n");
} /* __triggerOnEvent_recurse */


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

    __getStackPointer(&sp);

    expected = (sp - (RECURSION_COUNT * 8));

    recursive = 0;
    test__getStackPointer_recurse();

    if (_stack_ptr_ != expected)
    {
        printf(FAILED_WITH_WARNING);
        printf("  - expected (%p), but got (%p)\n", expected, _stack_ptr_);
    } /* if */
} /* test__getStackPointer */



void testOnEventGotoHandling(STATEPARAMS, int runCount)
/*
 * This tests ON [event] GOTO functionality. We set up an event handler,
 *  build up a couple of function calls on the stack, and trigger an event.
 *
 * This code checks to see if the event handler is called correctly, and if
 *  the base pointer is correct. If the base pointer (and therefore the stack)
 *  is fucked up, we dump some debug information to disk, and terminate the
 *  program gracefully, before it is terminated ungracefully for us by a
 *  corrupted stack.
 *
 * This function should be called more than once (but should be called several
 *  times, if you err on the side of caution), to verify that
 *  __(de)registerOnEventHandler() is working correctly. If not correctly, then
 *  correctly enough. testOnEventGotoRecurseHandling() is a more rigorous
 *  test of event handler (de)registration, since it stacks handlers.
 *
 *    params : runCount == count of times this function has been executed.
 *   returns : void.
 */
{
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
    char testVar3[] = TESTVAR_VALUE3;
    int landed = 0xEDFE;  /* looks like "FEED" in intel hexdump. */

    printf("Testing ON EVENT GOTO handling (run #%d)...\n", runCount);

    __getStackPointer(&_stack_ptr_);
    __getBasePointer(&_base_ptr_);
    __registerOnEventHandler(STATEARGS, &&errHandler, _base_ptr_,
                            _stack_ptr_,ONERROR);

    recursive = 0;
    __triggerOnEvent_recurse(STATEARGS, ONERROR, true);

    goto missedHandler;

errHandler:
    __getBasePointer(&_base_ptr2_); 
    __getStackPointer(&_stack_ptr2_);

    if (_base_ptr2_ != _base_ptr_)
    {
        printf("  - Base pointer is damaged.\n");
        printf("  - EBP inside error handler SHOULD BE (%p)\n", _base_ptr_);
        printf("  - EBP inside error handler IS (%p)\n", _base_ptr2_);
    } /* if */

    if (_stack_ptr2_ != _stack_ptr_)
    {
        printf("  - Stack pointer is damaged.\n");
        printf("  - ESP inside error handler SHOULD BE (%p)\n", _stack_ptr_);
        printf("  - ESP inside error handler IS (%p)\n", _stack_ptr2_);
    } /* if */

    if ((_base_ptr2_ != _base_ptr_) || (_stack_ptr2_ != _stack_ptr_))
    {
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
    goto endTest;

missedHandler:
    landed = 2;
    goto endTest;

endTest:
    switch (landed)
    {
        case 1:   /* good. */
            break;
        case 2:
            printf("  - Handler missed. Failed.\n");
            printf("  - Other tests will give incorrect results.\n"
                   "  - Must terminate testing.\n");
            exit(1);
            break;
        default:
            printf("  - Confused! Failed.\n");
            printf("  - Other tests will give incorrect results.\n"
                   "  - Must terminate testing.\n");
            exit(1);
            break;
    } /* switch */

    testVar3[0] = '\0';  /* stops compiler whining. */

    __deregisterOnEventHandlers(STATEARGS);
} /* testOnEventGotoHandling */



void testOnEventGotoRecurseHandling(STATEPARAMS, int runCount)
/*
 * This tests recursive ON [event] GOTO functionality. We set up an event
 *  handler, trigger an onEvent, and inside the handler, trigger another
 *  onEvent.
 *
 * This function checks the same things testOnEventsGotoHandling() does.
 *
 * This function should be called more than once (but should be called several
 *  times, if you err on the side of caution), to help verify that
 *  __(de)registerOnEventHandler() is working correctly.
 *
 *    params : runCount == count of times this function has been executed.
 *   returns : void.
 */
{
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
    char testVar3[] = TESTVAR_VALUE3;
    int landed = 0xEDFE;  /* looks like "FEED" in intel hexdump. */

    printf("Testing recursive ON EVENT GOTO handling (run #%d)...\n", runCount);

    recursive = 0;

    __getStackPointer(&_stack_ptr_);
    __getBasePointer(&_base_ptr_);
    __registerOnEventHandler(STATEARGS, &&errHandler, _base_ptr_,
                            _stack_ptr_,ONERROR);

    recursive = 0;
    __triggerOnEventByType(STATEARGS, ONERROR);
                                      /* no recurse since we use global var */
    goto missedHandler;

errHandler:
    __getBasePointer(&_base_ptr2_); 
    __getStackPointer(&_stack_ptr2_);

    if (_base_ptr2_ != _base_ptr_)
    {
        printf("  - Base pointer is damaged.\n");
        printf("  - EBP inside error handler SHOULD BE (%p)\n", _base_ptr_);
        printf("  - EBP inside error handler IS (%p)\n", _base_ptr2_);
    } /* if */

    if (_stack_ptr2_ != _stack_ptr_)
    {
        printf("  - Stack pointer is damaged.\n");
        printf("  - ESP inside error handler SHOULD BE (%p)\n", _stack_ptr_);
        printf("  - ESP inside error handler IS (%p)\n", _stack_ptr2_);
    } /* if */

    if ((_base_ptr2_ != _base_ptr_) || (_stack_ptr2_ != _stack_ptr_))
    {
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
    goto endTest;

missedHandler:
    landed = 2;
    goto endTest;

endTest:
    switch (landed)
    {
        case 1:   /* good. trigger again. */
            recursive++;
            if (recursive <= RECURSION_COUNT)
                __triggerOnEventByType(STATEARGS, ONERROR);
            break;
        case 2:
            printf("  - (Iteration #%d) Handler missed. Failed.\n", recursive);
            printf("  - Other tests will give incorrect results.\n"
                   "  - Must terminate testing.\n");
            exit(1);
            break;
        default:
            printf("  - (Iteration #%d) Confused! Failed.\n", recursive);
            printf("  - Other tests will give incorrect results.\n"
                   "  - Must terminate testing.\n");
            exit(1);
            break;
    } /* switch */

    testVar3[0] = '\0';  /* stops compiler whining. */

    __deregisterOnEventHandlers(STATEARGS);
} /* testOnEventGotoRecurseHandling */


void testResumeNext(STATEPARAMS, int runCount)
/*
 * Test RESUME NEXT command. We trigger an event, and see where control goes.
 *
 *  Run this multiple times, for the sake of the chaos factor.
 */
{
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
    char testVar3[] = TESTVAR_VALUE3;

    __setResumeStack;
    __setResumeInstructs(&&resumeZeroWrong, &&resumeNextRight);

    printf("Testing RESUME NEXT (run #%d)...\n", runCount);

    __getStackPointer(&_stack_ptr_);
    __getBasePointer(&_base_ptr_);
    __registerOnEventHandler(STATEARGS, &&resumeNextErrHandler,
                             _base_ptr_, _stack_ptr_, ONTIMER);

    recursive = 0;
    __triggerOnEvent_recurse(STATEARGS, ONTIMER, false);

    printf(FAILED);
    goto resumeNextEnd;

resumeNextErrHandler:
    __resumeNext(STATEARGS);
    printf(FAILED);
    goto resumeNextEnd;

resumeZeroWrong:    
    printf("  - Resumed ZERO instead of NEXT.\n");
    goto resumeNextEnd;

resumeNextRight:
    __getBasePointer(&_base_ptr2_); 
    __getStackPointer(&_stack_ptr2_);

    if (_base_ptr2_ != _base_ptr_)
    {
        printf("  - Base pointer is damaged.\n");
        printf("  - EBP inside error handler SHOULD BE (%p)\n", _base_ptr_);
        printf("  - EBP inside error handler IS (%p)\n", _base_ptr2_);
    } /* if */

    if (_stack_ptr2_ != _stack_ptr_)
    {
        printf("  - Stack pointer is damaged.\n");
        printf("  - ESP inside error handler SHOULD BE (%p)\n", _stack_ptr_);
        printf("  - ESP inside error handler IS (%p)\n", _stack_ptr2_);
    } /* if */

    if ((_base_ptr2_ != _base_ptr_) || (_stack_ptr2_ != _stack_ptr_))
    {
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

resumeNextEnd:
    __deregisterOnEventHandlers(STATEARGS);
} /* testResumeNext */


void testResumeZero(STATEPARAMS, int runCount)
/*
 * Test RESUME 0 command. We trigger an event, and see where control goes.
 *
 *  Run this multiple times, for the sake of the chaos factor.
 */
{
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
    char testVar3[] = TESTVAR_VALUE3;

    __setResumeStack;
    __setResumeInstructs(&&resumeZeroRight, &&resumeNextWrong);

    printf("Testing RESUME 0 (run #%d)...\n", runCount);

    __getStackPointer(&_stack_ptr_);
    __getBasePointer(&_base_ptr_);
    __registerOnEventHandler(STATEARGS, &&resumeZeroErrHandler,
                             _base_ptr_, _stack_ptr_, ONTIMER);

    recursive = 0;
    __triggerOnEvent_recurse(STATEARGS, ONTIMER, false);

    printf(FAILED);
    goto resumeZeroEnd;

resumeZeroErrHandler:
    __resumeZero(STATEARGS);
    printf(FAILED);
    goto resumeZeroEnd;

resumeNextWrong:
    printf("  - Resumed NEXT instead of ZERO.\n");
    goto resumeZeroEnd;

resumeZeroRight:
    __getBasePointer(&_base_ptr2_); 
    __getStackPointer(&_stack_ptr2_);

    if (_base_ptr2_ != _base_ptr_)
    {
        printf("  - Base pointer is damaged.\n");
        printf("  - EBP inside error handler SHOULD BE (%p)\n", _base_ptr_);
        printf("  - EBP inside error handler IS (%p)\n", _base_ptr2_);
    } /* if */

    if (_stack_ptr2_ != _stack_ptr_)
    {
        printf("  - Stack pointer is damaged.\n");
        printf("  - ESP inside error handler SHOULD BE (%p)\n", _stack_ptr_);
        printf("  - ESP inside error handler IS (%p)\n", _stack_ptr2_);
    } /* if */

    if ((_base_ptr2_ != _base_ptr_) || (_stack_ptr2_ != _stack_ptr_))
    {
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

resumeZeroEnd:
    __deregisterOnEventHandlers(STATEARGS);
} /* testResumeZero */


void testOnEventState(STATEPARAMS)
{
    if (__getOnEventsRecursionCount(STATEARGS) != 0)
    {
        printf("  - OnEvent was not cleaned up correctly.\n"
               "  - Later tests will probably fail inexplicably.\n");
    } /* if */
} /* testOnEventState */


void testOnEventHandling(STATEPARAMS)
/*
 * Test "on event" handling of various types.
 *
 *    params : void.
 *   returns : void.
 */
{
    int i;

    for (i = 1; i <= 3; i++)
    {
        testOnEventGotoHandling(STATEARGS, i);
        testOnEventState(STATEARGS);
    } /* for */

    for (i = 1; i <= 3; i++)
    {
        testOnEventGotoRecurseHandling(STATEARGS, i); 
        testOnEventState(STATEARGS);
    } /* for */

    for (i = 1; i <= 3; i++)
    {
        testResumeNext(STATEARGS, i);
        testOnEventState(STATEARGS);
    } /* for */

    for (i = 1; i <= 3; i++)
    {
        testResumeZero(STATEARGS, i);
        testOnEventState(STATEARGS);
    } /* for */
} /* testOnEventHandling */


void testOnEvents(STATEPARAMS)
/*
 * Test ON EVENT functionality in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n[TESTING ON EVENT HANDLING...]\n");
    test__getBasePointer();
    test__getStackPointer();
    testOnEventHandling(STATEARGS);
} /* testOnEvents */


#ifdef STANDALONE

#include "Initialize.h"

int main(void)
{
    __initBasicLib(NULLSTATEARGS, INITFLAG_NO_FLAGS);
    testOnEvents(NULLSTATEARGS);
    __deinitBasicLib(NULLSTATEARGS);
    return(0);
} /* main */

#endif

/* end of TestConversionFunctions.c ... */


