/*
 * Direct console support for BASIClib. Platform specific support for
 *  Linux (and soon, Win32.)
 *
 *    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "ConsoleFunctions.h"
#include "DirectConsole.h"



#ifdef LINUX   /* Linux-specific support. */

static int cons = -1;
static int x = 0;
static int y = 0;
static int lines = 0;
static int columns = 0;
static int fgColor = 7;
static int bgColor = 0;


static int __cons_openConsole(STATEPARAMS)
/*
 * Open the virtual console for read/write, if at a virtual console, and
 *  the kernel will permit it.
 *
 *    params : void.
 *   returns : false on error, true on success.
 */
{
    char *tty = ttyname(STDOUT_FILENO);
    char consDev[strlen(tty) + 20];
    unsigned char br[4];

    if (tty != NULL)
    {
            /* !!! check this conditional! */
        if ( (strncmp("/dev/tty", tty, 8) == 0) && (isdigit(tty[8])) )
        {
            strcpy(consDev, "/dev/vcsa");
            strcat(consDev, tty + 8);
            cons = open(consDev, O_RDWR);
            if (cons != -1)
            {
                if (read(cons, &br, 4) != 4)
                {
                    close(cons);
                    cons = -1;
                } /* if */
                else
                {
                    lines = (int) br[0];
                    columns = (int) br[1];
                    x = (int) br[2];
                    y = (int) br[3];
                } /* else */
            } /* if */
        } /* if */
    } /* if */

    return(cons);
} /* __cons_openConsole */


static void __cons_deinitConsole(STATEPARAMS)
{
    if (cons != -1)
        close(cons);
} /* __cons_deinitConsole */


static void __cons_vbpS_print(STATEPARAMS, PBasicString pStr)
{
} /* __cons_vbpS_print */


static void __cons_vbpii_viewPrint(STATEPARAMS, int top, int bottom)
{
} /* __cons_vbpii_viewPrint */


static void __cons_vbp_viewPrint(STATEPARAMS)
{
} /* __cons_vbp_viewPrint */

static void __cons_vbp_cls(STATEPARAMS)
{
} /* __cons_vbp_cls */

static int  __cons_vbi_csrline(STATEPARAMS)
{
} /* __cons_vbi_csrline */

static int  __cons_vbiA_pos(STATEPARAMS, void *pVar)
{
} /* __cons_vbiA_pos */

static void __cons_vbpiii_color(STATEPARAMS, int fore, int back, int bord)
{
} /* __cons_vbpiii_color */

#endif


#ifdef WIN32   /* stubs for now for win32. */
static int  __cons_openConsole(STATEPARAMS) { return(-1); }
static void __cons_deinitConsole(STATEPARAMS) {}
static void __cons_vbpS_print(STATEPARAMS, PBasicString pStr) {}
static void __cons_vbpii_viewPrint(STATEPARAMS, int top, int bottom) {}
static void __cons_vbp_viewPrint(STATEPARAMS) {}
static void __cons_vbp_cls(STATEPARAMS) {}
static int  __cons_vbi_csrline(STATEPARAMS) { return(0); }
static int  __cons_vbiA_pos(STATEPARAMS, void *pVar) { return(0); }
static void __cons_vbpiii_color(STATEPARAMS, int fore, int back, int bord) {}
#endif


boolean __initDirectConsole(STATEPARAMS)
/*
 * Attempt to initialize direct-to-console access.
 *
 *   params : void.
 *  returns : boolean true if initialized, false on error.
 */
{
    boolean retVal = false;

    if (__cons_openConsole(STATEARGS) != -1)
    {
        __deinitConsole = __cons_deinitConsole;
        __vbpS_print = __cons_vbpS_print;
        __vbpii_viewPrint = __cons_vbpii_viewPrint;
        __vbp_viewPrint = __cons_vbpii_viewPrint;
        __vbp_cls = __cons_vbp_cls;
        __vbi_csrline = __cons_vbi_csrline;
        __vbiA_pos = __cons_vbiA_pos;
        __vbpiii_color = __cons_vbpiii_color;
        retVal = true;
    } /* if */
} /* __initDirectConsole */

/* end of DirectConsole.c ... */

