/*
 * Declarations for abstract thread support in BASIClib.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#   include "StdBasic.h"

#else

#   ifndef _INCLUDE_THREADS_H_
#       define _INCLUDE_THREADS_H_

#       ifdef __cplusplus
            extern "C" {
#       endif

#       ifndef _REENTRANT
#           error _REENTRANT not defined!
#           error Please put -D_REENTRANT on the command line.
#           error  otherwise, the C library is NOT thread-safe!
#       endif /* _REENTRANT */

#       if ((defined LINUX) && (!defined LINUX_THREADS))
#           error LINUX_THREADS not defined!
#           error Please put -DLINUX_THREADS on the command line.
#           error  otherwise, the Garbage Collection code is NOT thread-safe!
#       endif /* LINUX_THREADS */

#       ifdef SINGLE_THREADED
            typedef unsigned long ThreadLock;
            typedef ThreadLock *PThreadLock;

#           define __terminateCurrentThread     vbp_end()
#           define __terminateThread(tidx)
#           define __waitForThreadToDie(tidx)
#           define __spinThread(fn, args)      -1
#           define __getThreadCount             1
#           define __getHighestThreadIndex      0
#           define __getCurrentThreadIndex      0
#           define __threadTimeslice
#           define __createThreadLock(pLock)    *(pLock) = 0
#           define __destroyThreadLock(pLock)
#           define __obtainThreadLock(pLock)
#           define __releaseThreadLock(pLock)

#       else   /* multithreaded support... */

#           include <pthread.h>

            typedef pthread_mutex_t ThreadLock;
            typedef pthread_mutex_t *PThreadLock;

            void __terminateCurrentThread_f(void);
            void __terminateThread_f(__integer tidx);
            void __waitForThreadToDie_f(__integer tidx);
            __integer __spinThread_f(void (*_fn)(void *x), void *_args);
            __integer __getThreadCount_f(void);
            __integer __getHighestThreadIndex_f(void);
            __integer __getCurrentThreadIndex_f(void);
            void __threadTimeslice_f(void);
            void __createThreadLock_f(PThreadLock pThreadLock);
            void __destroyThreadLock_f(PThreadLock pThreadLock);
            void __obtainThreadLock_f(PThreadLock pThreadLock);
            void __releaseThreadLock_f(PThreadLock pThreadLock);

#           define __terminateCurrentThread    __terminateCurrentThread_f()
#           define __terminateThread(tidx)     __terminateThread_f(tidx)
#           define __waitForThreadToDie(tidx)  __waitForThreadToDie_f(tidx)
#           define __spinThread(fn, args)      __spinThread_f(fn, args)
#           define __getThreadCount            __getThreadCount_f()
#           define __getHighestThreadIndex     __getHighestThreadIndex_f()
#           define __getCurrentThreadIndex     __getCurrentThreadIndex_f()
#           define __threadTimeslice           __threadTimeslice_f()
#           define __createThreadLock(pLock)   __createThreadLock_f(pLock)
#           define __destroyThreadLock(pLock)  __destroyThreadLock_f(pLock)
#           define __obtainThreadLock(pLock)   __obtainThreadLock_f(pLock)
#           define __releaseThreadLock(pLock)  __releaseThreadLock_f(pLock)
#       endif /* (!) SINGLE_THREADED */

        /* These exist in multi and single thread mode... */
        void __initThreads(void);
        void __deinitThreads(void);

#       ifdef __cplusplus
            }
#       endif

#   endif /* _INCLUDE_THREADS_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of Threads.h ... */

