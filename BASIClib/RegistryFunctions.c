/*
 * Registry-emulating API for BASIClib.
 *
 * This is the hack job of the century. Here's the basic design principle:
 *  The registry is stored in a text file (presumably
 *  /home/loginname/.vbslacker/registry), and is arranged like this:
 *
 *   AMy First Application Name
 *   SThis is a Section Name
 *   KSomeNumericDataKey
 *   VN39231
 *   KSomeStringDataKey
 *   VSThis is a string value!
 *   SAnother Section
 *   KMoreKeysHere
 *   VSMore values here...
 *   AAnother Application
 *
 *   etc...
 *
 *  This may improve for performance sake sometime, but I don't think
 *   this API is used for performance-critical code, but more for startup
 *   and shutdown of apps, where this will suffice. Obviously this won't
 *   emulate the whole Windows registry, and amen to that, but at least
 *   programs that need to store personal configuration data can do so
 *   in vbSlacker.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 *     Written by Ryan C. Gordon.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "RegistryFunctions.h"

static int registryFileHandle = -1;
static ThreadLock registryLock;


void __initRegistryFunctions(void)
/*
 * Setup registry-emulating functions. Attempt to find out where the
 *  VBHOME directory should go, and create/open a registry-alike file in
 *  there.
 *
 * We also create a ThreadLock, so multiple threads that do Registry
 *  accessing are serialized, for safety's sake.
 *
 * We will make the assumption that most programs will NOT need direct access
 *  to a registry, so if we can't open the registry for any reason, we still
 *  will not throw a runtime error unless the program later tries to access
 *  the registry in a way we can't handle.
 *
 *     params : void.
 *    returns : void.
 */
{
    char *vbHomeDir = __getUserAppDir();
    char regPath[strlen(vbHomeDir) + strlen(__FILENAME_REGISTRY) + 1];

    sprintf(regPath, "%s%s", vbHomeDir, __FILENAME_REGISTRY);

    __createThreadLock(&registryLock);

    if (registryFileHandle == -1)
        registryFileHandle = open(regPath, O_RDWR);
} /* __initRegistryFunctions */


void __deinitRegistryFunctions(void)
{
    if (registryFileHandle != -1)
    {
        close(registryFileHandle);
        registryFileHandle = -1;
    } /* if */

    __destroyThreadLock(&registryLock);
} /* __deinitRegistryFunctions */


void _vbpSS_deletesetting(PBasicString appName, PBasicString section)
{
    if (registryFileHandle == -1)
        __runtimeError(ERR_PERMISSION_DENIED);   /* !!! better error? */
    else
    {
        __obtainThreadLock(&registryLock);
        __releaseThreadLock(&registryLock);
    } /* else */
} /* _vbpSS_deletesetting */

__boolean seekRegistryEntry(appName, section) {return(true);}

void _vbpSSS_deletesetting(PBasicString appName,
                           PBasicString section,
                           PBasicString key)
{
    if (registryFileHandle == -1)
        __runtimeError(ERR_PERMISSION_DENIED);   /* !!! better error? */
    else
    {
        __obtainThreadLock(&registryLock);
        if (seekRegistryEntry(appName, section) == false)
        {
            __releaseThreadLock(&registryLock);
//            __runtimeError(ERR_
        } /* if */
        __releaseThreadLock(&registryLock);
    } /* else */
} /* _vbpSSS_deletesetting */


void _vbpSSSV_savesetting(PBasicString appName, PBasicString section,
                          PBasicString key, PVariant setting)
{
    if (registryFileHandle == -1)
        __runtimeError(ERR_PERMISSION_DENIED);   /* !!! better error? */
    else
    {
        __obtainThreadLock(&registryLock);
        __releaseThreadLock(&registryLock);
    } /* else */
} /* _vbpSSSV_savesetting */

/* end of RegistryFunctions.c ... */
