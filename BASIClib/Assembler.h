/*
 * Some lowlevel stuff that all vbSlacker code should include.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_
#   include "StdBasic.h"
#else

#   ifndef _INCLUDE_ASSEMBLER_H_
#       define _INCLUDE_ASSEMBLER_H_

#       ifdef __cplusplus
            extern "C" {
#       endif

#       define _INCLUDING_LOCAL_ASM_CODE_

#       if (defined __GNUC__)
#           if (defined __I386)
#               include "Asm_I386_GNU.h"
#           else
#               define ___NOASM___
#           endif

#       elif (defined __MSVC__)
#           if (defined __I386)
#               include "Asm_I386_MSVC.h"
#           else
#               define ___NOASM___
#           endif

#       else
#           define ___NOASM___
#       endif

#       undef _INCLUDING_LOCAL_ASM_CODE_

#       if (defined ___NOASM___)
#           error No ASM written for this platform/compiler, yet...
#       endif /* defined ___NOASM___ */

#       ifdef __cplusplus
            }
#       endif
#   endif /* defined _INCLUDE_ASSEMBLER_H_ */
#endif /* defined _INCLUDE_STDBASIC_H_ */

/* end of Assembler.h ... */


