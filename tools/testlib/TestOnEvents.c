/*
 * Test conversion functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
#include "Initialize.h"
#include "Threads.h"
#include "OnEvents.h"
#include "ErrorFunctions.h"
#include "Boolean.h"

#define TESTVAR_VALUE1 0xABCD
#define TESTVAR_VALUE2 0x1234
#define TESTVAR_VALUE3 "Look for this string in the data dump!"

#define RECURSION_COUNT 3

#define FAILED "  - Failed.\n"
#define FAILED_WITH_WARNING "  - Failed. Later tests will fail, too.\n"

int recursive = 0;

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


void __triggerOnEvent_recurse(OnEventTypeEnum evType, boolean printErr)
/*
 * This function calls itself RECURSION_COUNT times, to pile
 *  some data on the stack, then calls __triggerOnEvent(evType)...
 */
{
    recursive++;
    if (recursive == RECURSION_COUNT)
        __triggerOnEvent(evType);
    else
        __triggerOnEvent_recurse(evType, printErr);

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


void testOnEventGotoHandling(int runCount)
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
 *  correctly enough. testOnEventGotoStressing() is a more rigorous test of
 *  event handler (de)registration, since it stacks handlers.
 *
 *    params : runCount == count of times this function has been executed.
 *   returns : void.
 */
{
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
    char testVar3[] = TESTVAR_VALUE3;
    int landed = 0xEDFE;  /* looks like "FEED" in intel hexdump. */

    printf("Testing ON event GOTO handling (run #%d)...\n", runCount);

    __obtainThreadLock(&registerLock);
    __getStackPointer(&_stack_ptr_);
    __getBasePointer(&_base_ptr_);
    __registerOnEventHandler(&&errHandler, &runCount + sizeof (runCount),
                             _stack_ptr_, _base_ptr_, ONERROR);
    __releaseThreadLock(&registerLock);

    recursive = 0;
    __triggerOnEvent_recurse(ONERROR, true);

    goto missedHandler;

errHandler:
    __markOnEventHandlerAddr;
    if ((testVar1 != TESTVAR_VALUE1) || (testVar2 != TESTVAR_VALUE2) ||
        (strcmp(testVar3, TESTVAR_VALUE3) != 0))
    {
        __getBasePointer(&_base_ptr_); 
        __getStackPointer(&_stack_ptr_);
        printf("  - Base pointer is damaged.\n");
        printf("  - EPB inside error handler is (%p)\n", _base_ptr_);
        printf("  - ESP inside error handler is (%p)\n", _stack_ptr_);
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

    __deregisterOnEventHandler(&runCount + sizeof (runCount), ONERROR);
} /* testOnEventGotoHandling */



void testOnEventGotoRecurseHandling(int runCount)
/*
 * This tests recursive ON [event] GOTO functionality. We set up an event
 *  handler, trigger an onEvent, and inside the handler, trigger another
 *  onEvent.
 *
 * This function checks the same things testOnEventsGotoHandling() does.
 *
 * This function should be called more than once (but should be called several
 *  times, if you err on the side of caution), to verify that
 *  __(de)registerOnEventHandler() is working correctly. If not correctly, then
 *  correctly enough. testOnEventGotoStressing() is a more rigorous test of
 *  event handler (de)registration, since it stacks handlers.
 *
 *    params : runCount == count of times this function has been executed.
 *   returns : void.
 */
{
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
    char testVar3[] = TESTVAR_VALUE3;
    int landed = 0xEDFE;  /* looks like "FEED" in intel hexdump. */

    printf("Testing recursive ON event GOTO handling (run #%d)...\n", runCount);

    recursive = 0;

    __obtainThreadLock(&registerLock);
    __getStackPointer(&_stack_ptr_);
    __getBasePointer(&_base_ptr_);
    __registerOnEventHandler(&&errHandler, &runCount + sizeof (runCount),
                             _stack_ptr_, _base_ptr_, ONERROR);
    __releaseThreadLock(&registerLock);


    __triggerOnEvent(ONERROR);  /* no recursion so we can use global var */
    goto missedHandler;

errHandler:
    __markOnEventHandlerAddr;
    if ((testVar1 != TESTVAR_VALUE1) || (testVar2 != TESTVAR_VALUE2) ||
        (strcmp(testVar3, TESTVAR_VALUE3) != 0))
    {
        __getBasePointer(&_base_ptr_); 
        __getStackPointer(&_stack_ptr_);
        printf("  - (Iteration #%d) Base pointer is damaged.\n", recursive);
        printf("  - EPB inside error handler is (%p)\n", _base_ptr_);
        printf("  - ESP inside error handler is (%p)\n", _stack_ptr_);
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
                __triggerOnEvent(ONERROR);
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

    __deregisterOnEventHandler(&runCount + sizeof (runCount), ONTIMER);
} /* testOnEventGotoRecurseHandling */


void testResumeNext(int runCount)
/*
 * Test RESUME NEXT command. We trigger an event, and see where control goes.
 *
 *  Run this multiple times, for the sake of the chaos factor.
 */
{
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
    char testVar3[] = TESTVAR_VALUE3;

    printf("Testing RESUME NEXT (run #%d)...\n", runCount);

    __obtainThreadLock(&registerLock);
    __getStackPointer(&_stack_ptr_);
    __getBasePointer(&_base_ptr_);
    __registerOnEventHandler(&&resumeHere, &runCount + sizeof (runCount),
                             _stack_ptr_, _base_ptr_, ONTIMER);
    __releaseThreadLock(&registerLock);

    recursive = 0;
    __triggerOnEvent_recurse(ONTIMER, false);

    if ((testVar1 != TESTVAR_VALUE1) || (testVar2 != TESTVAR_VALUE2) ||
        (strcmp(testVar3, TESTVAR_VALUE3) != 0))
    {
        __getBasePointer(&_base_ptr_); 
        __getStackPointer(&_stack_ptr_);
        printf("  - (Iteration #%d) Base pointer is damaged.\n", recursive);
        printf("  - EPB inside error handler is (%p)\n", _base_ptr_);
        printf("  - ESP inside error handler is (%p)\n", _stack_ptr_);
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

    goto resumeSuccessful;

resumeHere:
    __markOnEventHandlerAddr;
    __resumeNext();
    printf(FAILED);

resumeSuccessful:

    __deregisterOnEventHandler(&runCount + sizeof (runCount), ONERROR);
} /* testResumeNext */


void testOnEventHandling(void)
/*
 * Test "on event" handling of various types.
 *
 *    params : void.
 *   returns : void.
 */
{
    int i;

    for (i = 1; i <= 3; i++)
        testOnEventGotoHandling(i);

    for (i = 1; i <= 3; i++)
        testOnEventGotoRecurseHandling(i);

    for (i = 1; i <= 3; i++)
        testResumeNext(i);
} /* testOnEventHandling */


void testOnEvents(void)
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
    testOnEventHandling();
} /* testOnEvents */


#ifdef STANDALONE

int main(void)
{
    __initBasicLib();
    testOnEvents();
    __deinitBasicLib();
    return(0);
} /* main */

#endif

/* end of TestConversionFunctions.c ... */


