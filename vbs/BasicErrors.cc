/*
 * Implementation of the BasicErrors class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicErrors.h"
#include "BasicContext.h"
#include <stdlib.h>

#define MAX_ERRORS      256

BasicErrors::BasicErrors(void *pBasicContext)
/*
 * Constructor
 *
 *     params : pBasicContext   ==  Pointer to a valid BasicContext object.
 *    returns : none
 */
{
    this->m_pBasicContext = pBasicContext;
    this->m_Errors = new BasicError*[MAX_ERRORS];
    m_ErrorCount = 0;           // Reset error count to none
}

BasicErrors::~BasicErrors()
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

    delete this->m_Errors;      // Delete the error array
}

void BasicErrors::AddError(BASIC_ERROR_CODE sErrorCode)
/*
 * Adds a new BasicError instance to the error collection and constructs it
 *  with the appropriate context and passed error code.
 *
 *     params : sErrorCode  ==  Error code to create BasicError object with.
 *    returns : none
 */
{
                                // Get a "correct type" pointer to our
                                //  BasicContext member.
    BasicContext *pBasicContext = (BasicContext*)this->m_pBasicContext;

                                // If we can still add errors
    if(this->m_ErrorCount < MAX_ERRORS)
    {
        
                                // Create the new error object in the next
                                //  index.
        this->m_Errors[this->m_ErrorCount] = 
            new BasicError(pBasicContext->GetContext(), sErrorCode);

        pBasicContext->RaiseEvent(BASIC_EVENT_ERROR, this->m_Errors[this->m_ErrorCount]);
        this->m_ErrorCount++;   // Increment counter...duh.
    }

                                // If we have one more error we're allowed
                                //  to add.
    if(this->m_ErrorCount == MAX_ERRORS - 1)
    {
                                // Force the last error to indicate that we've
                                //  reached the maximum error limit.
        this->AddError(BASIC_ERROR_MAX_ERRORS);
                                // This will tell any BasicCompiler objects
                                //  associated with the context to terminate
                                //  themselves.
        pBasicContext->SetTerminateFlag();
    }
}

short BasicErrors::GetErrorCount()
/*
 * Returns current number of BasicError objects added to collection
 *
 *     params : none
 *    returns : none
 */
{
    return this->m_ErrorCount;
}

BasicError *BasicErrors::GetError(short sIndex)
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