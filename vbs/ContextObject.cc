/*
 * Implementation of the ContextObject class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "ContextObject.h"
#include "string.h"

ContextObject::ContextObject(CONTEXT_TYPE ContextType, char *strContextName,
                                  ContextObject *pParentContext)
/*
 * Constructor
 *
 *     params : ContextType     ==  Type of context we're creating...yeah.
 *              strContextName  ==  Name to give context (function name, etc...)
 *              pParentContext  ==  Pointer to parent context.  NULL if none.
 *    returns : none
 */
{
    this->m_ContextType = ContextType;
    this->m_pParentContext = pParentContext;

                                // Allocate just enough memory for name and
                                //  null terminator.
    this->m_strContextName = new char[strlen(strContextName) + 1];
                                // Copy passed name to our private member.        
    strcpy(this->m_strContextName, strContextName);
}

ContextObject::~ContextObject()
/*
 * Destructor
 *
 *     params : none
 *    returns : none
 */
{
    delete this->m_strContextName;
}

ContextObject *ContextObject::GetParent()
/*
 * Returns a pointer to parent context
 *
 *     params : none
 *    returns : see description above.
 */
{
    return this->m_pParentContext;   
}

CONTEXT_TYPE ContextObject::GetType()
/*
 * Returns context type associated with this ContextObject instance.
 *
 *     params : none
 *    returns : see description above.
 */
{
    return this->m_ContextType;
}

char *ContextObject::GetName()
/*
 * Returns a pointer to name of the context.  This pointer is valid for as long
 *  as this ContextObject instance exists.
 *
 *     params : none
 *    returns : see description above.
 */
{
    return this->m_strContextName;
}

/* end of ContextObject.cpp */