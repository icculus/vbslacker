/*
 * Declarations for the BasicErrors class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICERRORS_H_
#define _INCLUDE_BASICERRORS_H_

#include "BasicError.h"
#include "boolean.h"

class BasicErrors
{
    public:
        BasicErrors(void *pBasicContext);
        ~BasicErrors();
        void AddError(BASIC_ERROR_CODE sErrorCode);
        short GetErrorCount();
        BasicError *GetError(short sIndex);
    
    private:
        short m_ErrorCount;     // Number of BasicError objects in collection
        BasicError **m_Errors;  // Collection of BasicError objects added

                                // Context object passed from compiler
        void *m_pBasicContext;
};

#endif

/* end of BasicErrors.h */