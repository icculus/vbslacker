/*
 * BASIClib Dynamic Loader API.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 *   This file written by Ryan C. Gordon.
 */

#include <stdio.h>
#include <stdlib.h>
#include "StdBasic.h"

#ifdef LINUX
#   include <dlfcn.h>
#endif

__DynamicBinaryHandle __dynLoadBinary(char *binaryName)
/*
 * Open a binary with dynamically loadable symbols. This binary's name
 *  is system dependent, but some amount of help may be given by
 *  passing your name through __dynPlatformBinaryName() first.
 *
 * Calling this with the name of an already-loaded binary will increase the
 *  reference count for that binary, and return the same handle.
 *  __dynFreeBinary() will not actually free that binary until the reference
 *  count drops back to zero. Reloaded a freed binary may or may not return
 *  the same handle.
 *
 * Each call to __dynLoadBinary() with the same name increases that binary's
 *  reference count. The initial loading of the binary creates a reference
 *  count of one.
 *
 *    params : binaryName == System-dependent binary name ("libm.so", etc.)
 *   returns : handle of opened binary on success, __dyn_NULL_HANDLE on error.
 */
{
    __DynamicBinaryHandle retVal = __dyn_NULL_HANDLE;
    void *rc = dlopen(binaryName, RTLD_LAZY);

    if (rc != NULL)
        retVal = (__DynamicBinaryHandle) rc;

    return(retVal);
} /* __dynLoadBinary */


void *__dynFindSymbol(__DynamicBinaryHandle handle, char *symbolName)
/*
 * Locate the address of a symbol in a loaded binary.
 *
 *    params : handle == handle returned from __dynLoadBinary().
 *             symbolName == name of symbol to locate in binary.
 *   returns : NULL if symbol not found, address of symbol otherwise.
 */
{
    return(dlsym(handle, symbolName));
} /* __dynFindSymbol */


void __dynFreeBinary(__DynamicBinaryHandle handle)
/*
 * Decrement the reference count of a dynamic binary, and if there are no
 *  more references, free the resources associated with it.
 *
 * Note that once the resources are freed, using any pointers associated with
 *  symbols from that binary (as returned by __dynFindSymbol()), will probably
 *  cause a segmentation fault, since the memory addressed no longer belongs
 *  to the current process. The binary may be reloaded with __dynLoadBinary()
 *  after being freed, but makes no promises that the previous pointers will,
 *  at that point, be accurate again.
 *
 *    params : handle == handle returned from __dynLoadBinary().
 *             symbolName == name of symbol to locate in binary.
 *   returns : NULL if symbol not found, address of symbol otherwise.
 */
{
    dlclose(handle);
} /* __dynFreeBinary */


char *__dynPlatformBinaryName(char *baseName)
/*
 * This function suggests a roughly best name for a dynamic binary, based
 *  on a base name. For example, with a basename of "gtk", under Linux, you'll
 *  get a return of "libgtk.so", whereas under Win32, you'll get "gtk.DLL".
 *
 * The memory used for the return value is dynamically allocated with malloc(),
 *  __memAlloc() is -NOT- used for this, so be sure to free() the retVal as
 *  soon as you are done with it!
 *
 * This function doesn't guarantee the suggested name exists, or is correct.
 *
 *     params : baseName == basename for dynamic binary.
 *    returns : suggested platform-dependent name. NULL on malloc() failure.
 */
{
    char *retVal = NULL;

    if (baseName == NULL)       /* sanity check. */
        return(NULL);

#   if (defined LINUX)
        retVal = malloc(strlen(baseName) + 7);
        if (retVal != NULL)
        {
            sprintf(retVal, "lib%s.so", baseName);
        } /* if */
#   elif ((defined WIN32) || (defined OS2))
        retVal = malloc(strlen(baseName) + 4);
        if (retVal != NULL)
        {
            strcpy(retVal, baseName);
            strcat(retVal, ".DLL");
        } /* if */
#   else
#       error Please add platform-specific details here!
#   endif

    return(retVal);
} /* __dynPlatformBinaryName */

/* end of DynamicLoader.c ... */

