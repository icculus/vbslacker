/*
 * Implementation of the ContextObject class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicContext.h"

void BasicContext::BasicContext()
/*
 * Constructor
 *
 *     params : none
 *    returns : none
 */
{
}

void BasicContext::~BasicContext()
/*
 * Destructor
 *
 *     params : none
 *    returns : none
 */
{
}

ContextObject *BasicContext::EnterContext(CONTEXT_TYPE Context,
                                          char *strContextName)
/*
 * Create a new ContextObject instance and set that as the "current" context.
 *
 *     params : ContextType     ==  Type of context we're creating...yeah.
 *              strContextName  ==  Name to give context (function name, etc...)
 *    returns : none
 */
{
}

void BasicContext::ExitContext()
{
}

inline ContextObject *BasicContext::GetContext()
{
    return this->m_pCurrentContext;
}

inline BasicErrors *BasicContext::GetErrors()
{
    return this->m_pBasicErrors;
}

inline BasicStatements *BasicContext::GetStatements()
{
    return this->m_pBasicStatements;
}

/* end of BasicContext.cpp */