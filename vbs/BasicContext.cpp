/*
 * Implementation of the ContextObject class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicContext.h"
#include <stdlib.h>

#define MAX_CONTEXT_OBJECTS     2048

BasicContext::BasicContext(BASIC_EVENT_HANDLER pEventHandler)
/*
 * Constructor
 *
 *     params : pEventHandle    ==  Pointer to an event handler function.
 *                                  This value is NULL if none is desired.
 *    returns : none
 */
{
                                // We're not in any context at startup
    this->m_pCurrentContext = NULL;
                                // No ContextObjects added yet.
    this->m_NumberOfContextObjects = 0;
 
    this->m_pBasicErrors = new BasicErrors(this);
    this->m_pBasicStatements = new BasicStatements(this);
    this->m_pContextObjects = new ContextObject*[MAX_CONTEXT_OBJECTS];
    this->m_TerminateFlag = FALSE;
    this->m_pEventHandler = pEventHandler;
}

BasicContext::~BasicContext()
/*
 * Destructor
 *
 *     params : none
 *    returns : none
 */
{
    short i;                    // Generic loop counter
    ContextObject **p;          // Pointer to current context object

    delete this->m_pBasicErrors;
    delete this->m_pBasicStatements;

                                // Loop through our ContextObject array
                                //  and delete all the ones we created.
    p = this->m_pContextObjects;
    for(i = 0; i < this->m_NumberOfContextObjects; i++)
    {
        delete (*p);
    }

                                // Now remove the array.
    delete this->m_pContextObjects;
}

ContextObject *BasicContext::EnterContext(CONTEXT_TYPE Context,
                                          char *strContextName)
/*
 * Create a new ContextObject instance and set that as the "current" context.
 *  The ContextObject instance returned is valid for as long as the
 *  BasicContext instance that created it is not destroyed.
 *
 *     params : ContextType     ==  Type of context we're creating...yeah.
 *              strContextName  ==  Name to give context (function name, etc...)
 *    returns : Pointer to the newly created ContextObject instance.
 */
{
    this->m_pCurrentContext = new ContextObject(Context, strContextName, this->m_pCurrentContext);
    return this->m_pCurrentContext;
}

void BasicContext::ExitContext()
/*
 * Exits the current context, but does not destroy the ContextObject instance.
 *
 *     params : none
 *    returns : none
 */
{
    this->m_pCurrentContext = this->m_pCurrentContext->GetParent();
}

ContextObject *BasicContext::GetContext()
/*
 * Returns current context.  NULL if no context.
 *
 *     params : none
 *    returns : see description above
 */
{
    return this->m_pCurrentContext;
}

BasicErrors *BasicContext::GetErrors()
/*
 * Returns collection of errors related to the BasicContext instance.
 *
 *     params : none
 *    returns : see description above
 */
{
    return this->m_pBasicErrors;
}

BasicStatements *BasicContext::GetStatements()
/*
 * Returns BasicStatements object associated with BasicContext instance.
 *
 *     params : none
 *    returns : see description above
 */
{
    return this->m_pBasicStatements;
}

BOOLEAN BasicContext::GetTerminateFlag()
/*
 * Returns compilation termination request status.
 *
 *     params : none
 *    returns : see description above
 */
{
    return this->m_TerminateFlag;
}

void BasicContext::SetTerminateFlag()
/*
 * Sets compilation termination request status.
 *
 *     params : none
 *    returns : see description above
 */
{
    this->m_TerminateFlag = TRUE;
}

void BasicContext::RaiseEvent(BASIC_EVENT Event, void *pData)
/*
 * Calls the appropriate event handler (if there is one)
 *
 *     params : Event   ==  Event to pass to event handler
 *              pData   ==  Pointer to data associated with event (if any)
 *    returns : none
 */
{
    if(this->m_pEventHandler != NULL)
        (*this->m_pEventHandler)(Event, pData);
}

/* end of BasicContext.cpp */