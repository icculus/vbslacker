/*
 * Declarations for the BasicError class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICERROR_H_
#define _INCLUDE_BASICERROR_H_

#include "ContextObject.h"

enum BASIC_ERROR_CODE {
    BASIC_ERROR_UNRECOGNIZED_FILE           =   0x00,
    BASIC_ERROR_FILE_OPEN                   =   0x01,
    BASIC_ERROR_FILE_READ                   =   0x02,
    BASIC_ERROR_UNKNOWN                     =   0x03
};

class BasicError
{
    public:
        void BasicError(ContextObject *pContextObject, 
            BASIC_ERROR_CODE sErrorCode);
        void ~BasicError();

        BASIC_ERROR_CODE GetErrorCode();
        ContextObject *GetContext();
        char *GetMessage();
    private:
                                // Error code associated with this object
        BASIC_ERROR_CODE m_sErrorCode;
                                // Context of the error
        ContextObject *m_pContextObject;
                                // Message created based on errorcode and
                                //  context.
        char *m_strMessage;     
};

#endif

/* end of BasicError.h */