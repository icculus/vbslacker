/*
 * Implementation of the BasicErrors class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicErrors.h"

#define MAX_ERRORS      256

void BasicErrors::BasicErrors(BasicContext *pContext)
/*
 * Constructor
 *
 *     params : pBasicContext   ==  Pointer to a valid BasicContext object.
 *    returns : none
 */
{
    this->m_pContext = pContext;
    this->m_Errors = new (BasicError*)[MAX_ERRORS];
    m_ErrorCount = 0;           // Reset error count to none
}

void BasicErrors::~BasicErrors()
/*
 * Destructor
 *
 *     params : none
 *    returns : none
 */
{
    BasicError **p;
    short i;

    p = this->m_Errors;         // Start at beginning of array

                                // Cycle through all error objects
    for(i = 0; i < this->m_ErrorCount; i++)
    {
        delete *p;              // Delete the BasicError object
        p++;                    // Increment to next BasicError object
    }

    delete this->m_ErrorCount;  // Delete the error array
}

BOOLEAN BasicErrors::AddError(BASIC_ERROR_CODE sErrorCode)
/*
 * Adds a new BasicError instance to the error collection and constructs it
 *  with the appropriate context and passed error code.
 *
 *     params : sErrorCode  ==  Error code to create BasicError object with.
 *    returns : TRUE if successful, otherwise FALSE.
 */
{
    BOOLEAN blnReturnValue;     // Value to return from function

                                // If no more errors are allowed to be added.
    if(this->m_ErrorCount = MAX_ERRORS)
        blnReturnValue = FALSE; // Return an error
    else                        // It's all good
    {
        
                                // Create the new error object in the next
                                //  index.
        *(this->m_Errors[this->m_ErrorCount]) = 
            new BasicError(this->m_pContext->GetContext(), sErrorCode);

        this->m_ErrorCount++;   // Increment counter...duh.
    }    
}

inline short BasicErrors::GetErrorCount()
/*
 * Returns current number of BasicError objects added to collection
 *
 *     params : none
 *    returns : none
 */
{
    return this->m_ErrorCount;
}

inline BasicError *BasicErrors::GetError(short sIndex)
/*
 * Returns BasicError object contained in collection based on the passed index.
 *
 *     params : sIndex  ==  Index of BasicError object in collection
 *    returns : If sIndex is valid, it returns a pointer to that object.
 *               Otherwise a NULL pointer is returned.
 */
{
    BasicError *pReturnValue;   // Value to return from function

                                // If index passed is invalid
    if(sIndex < 0 || sIndex >= this->m_ErrorCount)
        pReturnValue = NULL;
    else                        // Index is valid
        pReturnValue = m_Errors[sIndex];

    return pReturnValue;        // Return value from function
}

/* end of BasicErrors.cpp */