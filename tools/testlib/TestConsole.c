#include <stdio.h>      /* for sprintf()... */
#include <unistd.h>     /* for sleep()...   */
#include "BasicLib.h"


void testPrinting(void)
{
    __byte consName[30];
    PBasicString pStr;

    __getConsoleDriverName(consName, sizeof (consName));

    __printNewLine();
    __printAsciz("[If you can read this, the console has initialized.]");
    __printNewLine();
    __printAsciz(" (...and __printAsciz() and __printNewLine() work.)");
    __printNewLine();
    __printAsciz(" Console driver: \"");
    __printAsciz(consName);
    __printAsciz("\"");
    __printNewLine();
    __printNewLine();
    __printNChars("If this text is 53 characters, __printNChars() works.", 53);
    __printNewLine();
    __printAsciz("12345678901234567890123456789012345678901234567890123");
    __printNewLine();
    __printNewLine();

    __printAsciz("This is a really long line of text to see if text wrapping"
                 " is on the up-and-up; this should be a complete sentence,"
                 " complete with semicolon.");
    __printNewLine();

    pStr = __createString("If you can see this, _vbpS_print() works.", false);
    _vbpS_print(pStr);
    __printNewLine();
    __freeString(pStr);

#warning printing Variants needs to be tested, still.
    __printAsciz("#warning: printing Variants needs to be tested, still.");
    __printNewLine();

#warning printing non-Strings needs to be tested, still.
    __printAsciz("#warning: printing non-Strings needs to be tested, still.");
    __printNewLine();

    __printAsciz("Done testing basic printing functions. Ten second delay...");
    sleep(10);
} /* testPrinting */


void testClearing(void)
{
    __ONERRORVARS;
    __ONERRORINIT();
    __setOnErrorHandler(_cls_test_error_handler_);
    __setInstructs(_start_cls_test_, _end_cls_test_);

__insertLineLabel(_start_cls_test_);
    _vbp_cls();
    __printAsciz("Screen should have cleared via CLS. ");
__insertLineLabel(_end_cls_test_);
    __printAsciz("Three second delay...");
    sleep(3);
    __exitCleanupOnError();
    return;

__insertLineLabel(_cls_test_error_handler_);
    __printNewLine();
    __printAsciz("Runtime Error: CLS not supported?");
    __printNewLine();
    __printNewLine();
    __resumeNext();
} /* testClearing */


void testLocate(void)
{
    __ONERRORVARS;
    __ONERRORINIT();

    _vbp_cls();
    __printAsciz("123456789012345678901234567890");
    __printNewLine();
    __printAsciz("2");
    __printNewLine();
    __printAsciz("3");
    __printNewLine();
    __printAsciz("4");
    __printNewLine();
    __printAsciz("5");
    __printNewLine();
    __printAsciz("6");
    __printNewLine();
    __printAsciz("7");
    __printNewLine();
    __printAsciz("8");
    __printNewLine();
    __printAsciz("9");
    __printNewLine();
    __printAsciz("10");
    __printNewLine();
    __printNewLine();
    __printAsciz("Testing LOCATE...");
    __printNewLine();
    __printAsciz("  - The 'X' should be at (x=4, y=5)...");
    __printNewLine();
    __printAsciz("  - The 'Y' should be at (x=5, y=8)...");
    __printNewLine();
    __printAsciz("  - The 'Z' should be at (x=9, y=8)...");


    __setOnErrorHandler(_locate_test_error_handler_);
    __setInstructs(_start_locate_test_, _end_locate_test_);
__insertLineLabel(_start_locate_test_);
    _vbpii_locate(5, 4);
    __printAsciz("X");
    _vbpiN_locate(8);
    __printAsciz("Y");
    _vbpNi_locate(9);
    __printAsciz("Z");
    _vbpii_locate(17, 1);

__insertLineLabel(_end_locate_test_);
    __printAsciz("  ...delaying 10 seconds...");
    __printNewLine();
    sleep(10);

    __exitCleanupOnError();
    return;

__insertLineLabel(_locate_test_error_handler_);
    __printNewLine();
    __printAsciz("Runtime Error: LOCATE not supported?");
    __printNewLine();
    __printNewLine();
    __resumeNext();
} /* testLocate */


void testScrolling(void)
{
    __integer i;
    __integer n;
    __integer j = 0;
    __byte buf[90];

    _vbp_cls();
    __printAsciz("About to test scrolling text in 3 seconds...");
    sleep(3);
    _vbp_cls();

    for (i = 0; i < 200; i++)
    {
        for (n = 0; n < j; n++)
            buf[n] = ' ';
        sprintf(buf + n, "%d", j);
        __printAsciz(buf);
        __printNewLine();
        j++;
        if (j > 70)
            j = 0;
    } /* for */
    sleep(3);
} /* testScrolling */


void testViewPrint()
{
    __ONERRORVARS;
    __integer i;

    __ONERRORINIT();
    _vbp_cls();
    for (i = 0; i < 300; i++)
        __printAsciz("abcdefghijklmnopqrstuvwxyz");


    __setOnErrorHandler(_viewprint_test_error_handler_);
    __setInstructs(_start_viewprint_test_, _end_viewprint_test_);
__insertLineLabel(_start_viewprint_test_);
    _vbpii_viewPrint(10, 15);
    _vbp_cls();
    __printAsciz("view print: Lines 10 - 15 should be blank."
                 " (except this text.)");
    sleep(5);
    __printNewLine();
    testScrolling();
    _vbp_viewPrint();
    _vbp_cls();

__insertLineLabel(_end_viewprint_test_);
    __printAsciz("Whole console should be clear, now...except this text.");
    sleep(3);
    __exitCleanupOnError();
    return;

__insertLineLabel(_viewprint_test_error_handler_);
    __printNewLine();
    __printAsciz("Runtime Error: VIEW PRINT not supported?");
    __printNewLine();
    __printNewLine();
    __resumeNext();
} /* testViewPrint */


void testColor(void)
{
    __ONERRORVARS;
    int i;
    char buffer[10];

    __ONERRORINIT();

    _vbp_cls();
    __printAsciz("About to test COLOR command...");
    __printNewLine();
    __printNewLine();

    __setOnErrorHandler(_color_test_error_handler_);
    __setInstructs(_start_color_test_, _end_color_test_);

__insertLineLabel(_start_color_test_);
    for (i = 0; i <= 15; i++)
    {
        sprintf(buffer, " %d ", i);
        _vbpi_color(i);
        __printAsciz(buffer);
    } /* for */

    _vbpi_color(7);  /* default color? */
    __printNewLine();
    __printNewLine();

    __printAsciz("Color testing complete. ");
__insertLineLabel(_end_color_test_);

    __printAsciz("Waiting 5 seconds...");
    __printNewLine();
    sleep(5);
    __exitCleanupOnError();
    return;

__insertLineLabel(_color_test_error_handler_);
    __printNewLine();
    __printAsciz("Runtime Error: COLOR not supported?");
    __printNewLine();
    __printNewLine();
    __resumeNext();
} /* testColor */


void testBeep(void)
{
    __ONERRORVARS;
    __ONERRORINIT();

    _vbp_cls();

    __setOnErrorHandler(_beep_test_error_handler_);
    __setInstructs(_start_beep_test_, _end_beep_test_);

__insertLineLabel(_start_beep_test_);
    __printAsciz("...here comes a beep in 3 seconds...");
    sleep(3);
    _vbp_beep();
    __printAsciz("There it was!");
    __printNewLine();

__insertLineLabel(_end_beep_test_);
    __printAsciz("BEEP testing complete. Three second delay...");
    __printNewLine();
    sleep(3);
    __exitCleanupOnError();
    return;

__insertLineLabel(_beep_test_error_handler_);
    __printNewLine();
    __printAsciz("Runtime Error: BEEP not supported?");
    __printNewLine();
    __printNewLine();
    __resumeNext();
} /* testBeep */

void testSound(void)
{
    int i;
    __ONERRORVARS;
    __ONERRORINIT();

    _vbp_cls();

    __setOnErrorHandler(_sound_test_error_handler_);
    __setInstructs(_start_sound_test_, _end_sound_test_);

__insertLineLabel(_start_sound_test_);
    __printAsciz("...here comes a raising sound in 3 seconds...");
    sleep(3);

        /* This is almost right from QBASIC's example code... */
    for (i = 440; i <= 1000; i += 5)
        _vbpif_sound(i, ((__single) i) / 1000.0);

    __printAsciz("There it was!");
    __printNewLine();

__insertLineLabel(_end_sound_test_);
    __printAsciz("SOUND testing complete. Three second delay...");
    __printNewLine();
    sleep(3);
    __exitCleanupOnError();
    return;

__insertLineLabel(_sound_test_error_handler_);
    __printNewLine();
    __printAsciz("Runtime Error: SOUND not supported?");
    __printNewLine();
    __printNewLine();
    __resumeNext();
} /* testSound */


void cleanup(void)
{
    _vbp_cls();
    __printAsciz("Console testing complete.");
    __printNewLine();
    __printNewLine();
} /* cleanup */


void testConsole(void)
{
//    testPrinting();
//    testClearing();
//    testLocate();
//    testScrolling();
//    testViewPrint();
//    testColor();
    testBeep();
//    testSound();
    cleanup();
} /* testConsole */


int main(int argc, char **argv, char **envp)
{
    __initBasicLib(INITFLAG_NO_FLAGS, argc, argv, envp);
    testConsole();
    return(0);
} /* main */

/* end of TestConsole.c ... */
