#include <unistd.h>
#include "BasicLib.h"


void testPrinting(void)
{
    __byte consName[30];
    PBasicString pStr;

    __getConsoleHandlerName(consName, sizeof (consName));

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
    _vbp_cls();
    __printAsciz("Screen should have cleared via CLS. Three second delay...");
    sleep(3);
} /* testClearing */


void testLocate(void)
{
#if 0
    _vbp_cls();
    __printNewLine();
    __printAsciz("Testing LOCATE...");
    _vbpii_locate(5
#endif
} /* testLocate */


void testScrolling(void)
{
    __integer i;
    __integer n;
    __integer j = 0;
    __byte buf[10];

    __printNewLine();
    __printAsciz("About to test scrolling text in 3 seconds...");
    sleep(3);
    _vbp_cls();

    for (i = 0; i < 200; i++)
    {
        for (n = 0; n < j; n++)
            __printAsciz(" ");
        sprintf(buf, "%d", j);
        __printAsciz(buf);
        __printNewLine();
        j++;
        if (j > 70)
            j = 0;
    } /* for */
} /* testScrolling */


void testConsole(void)
{
    testPrinting();
    testClearing();
    testLocate();
    testScrolling();
} /* testConsole */


int main(int argc, char **argv)
{
    void *base;
    __getBasePointer(base);
    __initBasicLib(base, INITFLAG_NO_FLAGS, argc, argv);
    testConsole();
    __deinitBasicLib();
    return(0);
} /* main */

/* end of TestConsole.c ... */
