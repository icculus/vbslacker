/*
 * Implementation of the BasicError class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicError.h"

#define NUMBER_OF_ERRORS    4

static char *ErrorMessages[NUMBER_OF_ERRORS] = {
    "Unrecognized file type.",
    "File could not be open.",
    "File could not be read.",
    "***UNKNOWN ERROR***",
};

void BasicError::BasicError(ContextObject *pContextObject, BASIC_ERROR_CODE sErrorCode)
/*
 * Constructor
 *
 *     params : pContextObject  ==  Pointer to a valid context object
 *              sErrorCode      ==  Error code constant to create object with
 *    returns : none
 */
{
                                // If errorcode was invalid, we'll set the
                                //  error message as being unknown.
    if(sErrorCode < NUMBER_OF_ERRORS || sErrorCode < 0)
        this->m_strMessage = ErrorMessages[BASIC_ERROR_UNKNOWN];
    else                        // Set message based on error code.
        this->m_strMessage = ErrorMessages[sErrorCode];

                                // Save errorcode into object instance.  Even
                                //  if the errorcode is unknown or invalid,
                                //  we're still interested in the errorcode
                                //  passed to this object instance.
    this->m_sErrorCode = sErrorCode;
    this->m_pContextObject = pContextObject;
}

void BasicError::~BasicError()
/*
 * Destructor
 *
 *     params : none
 *    returns : none
 */
{
}

inline BASIC_ERROR_CODE BasicError::GetErrorCode()
/*
 * Returns error code associated with BasicError object upon creation
 *
 *     params : none
 *    returns : see function description above
 */
{
    return this->m_sErrorCode;
}

inline ContextObject *BasicError::GetContext()
/*
 * Returns ContextObject associated with BasicError object upon creation
 *
 *     params : none
 *    returns : see function description above.
 */
{
    return this->m_pContextObject;
}

inline char *BasicError::GetMessage()
/*
 * Returns string message associated with error code.  The message pointer that
 *  is returned is valid for the duration of the program since it points to a
 *  static array.
 *
 *     params : none
 *    returns : see function description above.
 */
{
    return this->m_strMessage;
}

/* end of BasicError.cpp */
