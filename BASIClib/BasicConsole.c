/*
 * Internal console functions for BASIClib.
 *
 *    Copyright (c) 2000 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "BasicConsole.h"

__vbConsoleEntries consoleEntries;

static __DynamicBinaryHandle consDriverHandle = __dyn_NULL_HANDLE;

static inline void convertToUnixNewlines(char *buffer, int size)
/*
 * Rather than screw around with various newline signifiers, we just
 *  convert all 0x0D's to 0x0A's. Blank lines get ignored, so this is
 *  safe in this case.
 *
 *     params : buffer == where contents of config file are stored.
 *              size == number of bytes allocated to (buffer).
 *    returns : void.
 */
{
    int i;
    for (i = 0; i < size; i++)
    {
        if (buffer[i] == 0x0D)  // ASCII carriage return.
            buffer[i] = 0x0A;   // ASCII linefeed.
    } /* for */

        /* remove blank lines... */
    for (i = 0; i < size - 1; i++)
    {
        if ((buffer[i] == 0x0A) && (buffer[i + 1] == 0x0A))
        {
            memmove(buffer + i, buffer + i + 1, size - i);
            size--;
            i--;
        } // if
    } /* for */
} /* convertToUnixNewlines */


static inline char *getConsoleConfigBuffer(char *cfgfname)
/*
 * Read the whole config file into a block of memory, and make sure there
 *  are only unix-style newlines in the buffer.
 *
 *     param : cfgfname == config filename.
 *   returns : malloc()'d buffer. free() it when done.
 */
{
    struct stat statbuf;
    int fd;
    char *buffer;
    int rb;

    if (stat(cfgfname, &statbuf) == -1)
        return(NULL);

    fd = open(cfgfname, O_RDONLY);
    if (fd < 0)
        return(NULL);

    buffer = malloc(statbuf.st_size + 1);
    if (buffer == NULL)
    {
        close(fd);
        return(NULL);
    } /* if */
    
    rb = read(fd, buffer, statbuf.st_size);
    *(buffer + rb) = '\0';
    close(fd);

    convertToUnixNewlines(buffer, rb);
    return(buffer);
} /* getConsoleConfigBuffer */


static inline __boolean tryToInitConsoleDriver(char *lib, char *entry)
/*
 * At this point, we've got the basename for the console driver's
 *  library, and the identifier of the init entry point. Attempt to
 *  call that entry point, and see if it succeeds.
 *
 *     params : lib == basename of console driver's shared library.
 *              entry == identifier of driver's init entry point.
 *    returns : true if the driver initialized successfully, false otherwise.
 */
{
    __DynamicBinaryHandle handle;
    char *ptr = NULL;
    __boolean (*func)(__vbConsoleEntries *);

        /* I wonder if this is faster than "if (strcmp(lib, "$") == 0)" ... */
    if ((*lib == '$') && (*(lib + 1) == '\0'))
        handle = __dynLoadBinary(NULL);
    else
    {
        ptr = __dynPlatformBinaryName(lib);
        if (ptr == NULL)
            return(false);
        handle = __dynLoadBinary(ptr);
        free(ptr);
    } /* else */

    if (handle == __dyn_NULL_HANDLE)
        return(false);

    func = __dynFindSymbol(handle, entry);
    if (func == NULL)
    {
        __dynFreeBinary(handle);
        return(false);
    } /* if */

    if (func(&consoleEntries) == false)
    {
        __dynFreeBinary(handle);
        return(false);
    } /* if */

    consDriverHandle = handle;        /* Eureka! */
    return(true);
} /* tryToInitConsoleDriver */


static inline __boolean parseConsoleConfig(const char *cfgfname, int mode)
/*
 * Read the entries from the vbslacker.cons file, and try to find a driver
 *  that will initialize successfully for screen mode (mode).
 *
 * The .cons file is currently very simple. The format is:
 *
 *  mode libname initEntryName
 *
 * Where mode is a number that specifies the screen mode (argument to SCREEN
 *  function). libname is the shared library where the driver exists. This
 *  should be something like "VBcons", which on Linux becomes "libVBcons.so",
 *  and VBcons.dll on Win32, etc. The library search path is examined to find
 *  the library. If it's found and successfully dlopen()'d, and it contains the
 *  symbol initEntryName, then that symbol is called. If the symbol returns
 *  (true), then we use that as a driver, and expect that it filled in the
 *  __vbConsoleEntries structure. Otherwise, we'll try the next in the list.
 *
 *      params : cfgfname == configuration file name.
 *               mode == screen mode to try to initialize.
 *     returns : true if any console driver successfully initialized.
 */
{
    char *buffer = getConsoleConfigBuffer((char *) cfgfname);
    __boolean retVal = false;
    char *thisptr = buffer;
    char *nextptr;
    char *spaceptr;
    int val;

    while ((thisptr != NULL) && (retVal == false))
    {
        nextptr = strchr(thisptr, '\n'); /* look for a newline. */
        if (nextptr != NULL)
        {
            *nextptr = '\0';  /* null-terminate the string. */
            nextptr++;
        } /* if */

        spaceptr = strchr(thisptr, ' '); /* look for a space. */
        if (spaceptr != NULL)
        {
            *spaceptr = '\0';  /* null-terminate the string. */
            val = atoi(thisptr);
            if (val == mode)
            {
                thisptr = spaceptr + 1;
                spaceptr = strchr(thisptr, ' ');
                if (spaceptr != NULL)
                {
                    *spaceptr = '\0';  /* null-terminate the string. */
                    retVal = tryToInitConsoleDriver(thisptr, spaceptr + 1);
                } /* if */
            } /* if */
        } /* if */

        thisptr = nextptr;
    } /* while */

    if (buffer != NULL)
        free(buffer);

    return(retVal);
} /* readConsoleConfig */


static inline const char *getDefaultConsoleConfigFileName(__boolean withDir)
/*
 * Return the standard name of the console config file. This should return
 *  a pointer to STATIC MEMORY, so you can return a literal string, if you
 *  want. The reciever of the return value from this function should consider
 *  that memory to be READ ONLY.
 *
 *     params : withDir == Should the returned string have the standard
 *                         directory and the filename, or just the filename?
 *    returns : see above.
 */
{
#   ifdef UNIX
        if (withDir == true)
            return("/etc/vbslacker.cons");
        else
            return("vbslacker.cons");
#   elif (defined WIN32)
#       error needs API to find Windows dir.
#   elif (defined OS2)
#       error needs API to find OS2 main dir.
#   endif
} /* getDefaultLangFileName */


static inline closeConsoleDriver(void)
{
    if (consoleEntries.__deinitConsoleDriver != NULL)
        consoleEntries.__deinitConsoleDriver();

    memset(&consoleEntries, '\0', sizeof (__vbConsoleEntries));

    if (consDriverHandle != __dyn_NULL_HANDLE)
    {
        __dynFreeBinary(consDriverHandle);
        consDriverHandle = __dyn_NULL_HANDLE;
    } /* if */
} /* closeConsoleDriver */


__boolean __vbStartConsoleMode(char *vbHomeDir, int mode)
/*
 * Run down the possible console drivers, in order of precedence,
 *  until we either find one that works, or we run out of options.
 *  If we run out of options, a runtime error is thrown.
 *
 *     params : vbHomeDir == vb UserAppDir.
 *    returns : void.
 */
{
    char *cfgFileName = (char *) getDefaultConsoleConfigFileName(false);
    char cfgPath[strlen(vbHomeDir) + strlen(cfgFileName) + 1];
    __boolean retVal = true;

    closeConsoleDriver();  /* safe at startup, too. */

    sprintf(cfgPath, "%s%s", vbHomeDir, cfgFileName);

    if ((parseConsoleConfig(cfgPath, 0) == false) &&
        (parseConsoleConfig(getDefaultConsoleConfigFileName(true), 0)) == false)
    {
        consDriverHandle = __dyn_NULL_HANDLE;
        retVal = false;
    } /* if */

    return(retVal);
} /* __vbStartConsoleMode */


void __initConsole(char *vbHomeDir)
/*
 * Initialize the console subsystem.
 *
 *     params : vbHomeDir == vb UserAppDir.
 *    returns : void.
 */
{
    if (__vbStartConsoleMode(vbHomeDir, 0) == false)
        __runtimeError(ERR_CANNOT_CONTINUE);
} /* __initConsole */


void __deinitConsole(void)
/*
 * Clean up, free libraries, etc...
 *
 *      params : void.
 *     returns : void.
 */
{
    closeConsoleDriver();
} /* __deinitConsole */

/* end of BasicConsole.c ... */

