/*
 * Routines for dynamic loading of internal BASIClib strings. This
 *  allows language-independence for the guts of BASIClib.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 *   This file written by Ryan C. Gordon.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Language.h"


static char *__BASIClibStrings[(__BASIClibStringIndex) _TOTAL_STRING_COUNT_];
static __boolean initializedStrings = false;

static inline const char *getDefaultLangFileName(__boolean withDir)
/*
 * Return the standard name of the language file. This should return
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
            return("/etc/vbslacker.lang");
        else
            return("vbslacker.lang");
#   elif (defined WIN32)
#       error needs API to find Windows dir.
#   elif (defined OS2)
#       error needs API to find OS2 main dir.
#   endif
} /* getDefaultLangFileName */


static inline FILE *openLangFile(char *vbHomeDir)
/*
 * Open the correct langfile.
 *
 * The file is read from the vbslacker "user app dir" first, and failing
 * that, it is read from /etc/vbslacker.lang or an equivalent. Check
 * getDefaultLangFileName() for your platform's default langfile.
 *
 *   params : void.
 *  returns : ANSI C file stream for reading langfile.
 *   throws : Runtime error
 *
 */
{
    FILE *retVal = NULL;
    char *langFileName = getDefaultLangFileName(false);
    char *langPath = malloc(strlen(vbHomeDir) + strlen(langFileName) + 1);

    if (langPath == NULL)
        __runtimeError(ERR_OUT_OF_MEMORY);

    sprintf(langPath, "%s%s", vbHomeDir, langFileName);
    retVal = fopen(langPath, "r");
    free(langPath);

    if (retVal == NULL)
    {
        retVal = fopen(getDefaultLangFileName(true), "r");
        if (retVal == NULL)
            __runtimeError(ERR_CANNOT_CONTINUE);  /* !!! better error? */
    } /* if */

    return(retVal);
} /* openLangFile */


static void trimWhiteSpace(char *str)
{
    int end = -1;
    int i;
    int max = strlen(str);

    for (i = 0; end == -1; i++)
    {
        if ((str[i] != ' ') && (str[i] != '\t'))
            end = i;
    } /* for */

        /* Move string to the left, overwriting whitespace chars... */
    if (end != 0)
        memmove(str, str + end, strlen(str + end) + 1);

    max = strlen(str);
    end = -1;

    for (i = max; ((i >= 0) && (end == -1)); i--)
    {
        if ((str[i] != ' ') && (str[i] != '\t'))
            end = i;
    } /* for */

    if (end != -1)
        str[end + 1] = '\0';  /* terminate at start of whitespace. */
} /* trimWhiteSpace */


static char *readAndParseLanguageString(FILE *langFileStream)
{
    int rc;
    int strSize = 0;
    int readCount = 0;
    char *str = NULL;
    __boolean getOut = false;

    do
    {
        if (strSize <= readCount)
        {
            strSize += 100;
            str = realloc(str, strSize);
        } /* if */

        rc = fgetc(langFileStream);
        if ((rc == -1) || ((char) rc == '\r') || ((char) rc == '\n'))
            getOut = true;
        else
        {
            str[readCount] = (char) rc;
            readCount++;
        } /* else */
    } while (getOut == false);

    str[readCount] = '\0';  /* null-terminate the data we've read. */

    trimWhiteSpace(str);

           /* comment or blank line? Recursive call gets next line... */
    if ((str[0] == '\0') || (str[0] == '#'))
    {
        free(str);
        str = readAndParseLanguageString(langFileStream);
    } /* if */
    else
        realloc(str, strlen(str) + 1);

    return(str);
} /* readAndParseLanguageString */


const char *__getLanguageString(__BASIClibStringIndex idx)
{
    char *retVal = NULL;

    if (initializedStrings == true)
        retVal = __BASIClibStrings[idx];

    return(retVal);
} /* __getLanguageString */


void __initLanguage(char *vbHomeDir)
/*
 * Read internal BASIClib strings from a language-independent text file.
 *
 * The langfile format is discussed in /vbslacker/docs/adding_languages.txt ...
 *
 *  This is called VERY early in the initialization process, so you
 *  may not use the vast majority of BASIClib's functionality.
 *  Runtime errors are "preinit" and ugly, but work. The memory manager
 *  is not available at this point (use malloc().), __getUserAppDir() is
 *  available, but (vbHomeDir) is what that would return. Generally this is
 *  one of few places in BASIClib where you are encouraged to use C runtime
 *  library functions in favor of BASIClib equivalents.
 *
 *     params : vbHomeDir == vb UserAppDir.
 *    returns : void
 */
{
    FILE *langFileStream;
    unsigned int i;

    if (initializedStrings == false)
    {
        langFileStream = openLangFile(vbHomeDir);

        for (i = 0; i < (__BASIClibStringIndex) _TOTAL_STRING_COUNT_; i++)
        {
            __BASIClibStrings[i] = readAndParseLanguageString(langFileStream);
            if (ferror(langFileStream))
                __runtimeError(ERR_CANNOT_CONTINUE);
            if (feof(langFileStream))
            {
                if (i != (__BASIClibStringIndex) _TOTAL_STRING_COUNT_ - 1)

                    __runtimeError(ERR_CANNOT_CONTINUE);
            } /* if */
        } /* for */

        fclose(langFileStream);
        initializedStrings = true;
    } /* if */
} /* __initLanguage */


void __deinitLanguage(void)
/*
 * Free up all the allocated language strings.
 *
 *     params : void.
 *    returns : void.
 */
{
    unsigned int i;

    if (initializedStrings == true)
    {
        for (i = 0; i < (__BASIClibStringIndex) _TOTAL_STRING_COUNT_; i++)
        {
            if (__BASIClibStrings[i] != NULL)
            {
                free(__BASIClibStrings[i]);
                __BASIClibStrings[i] = NULL;
            } /* if */
        } /* for */
    } /* if */

    initializedStrings = false;
} /* __deinitLanguage */

/* end if Language.c ... */

