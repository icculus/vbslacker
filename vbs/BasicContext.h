/*
 * Declarations for the ContextObject class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICCONTEXT_H_
#define _INCLUDE_BASICCONTEXT_H_

#include "ContextObject.h"

class BasicContext
{
    public:
        void BasicContext();
        void ~BasicContext();

        ContextObject *EnterContext(CONTEXT_TYPE Context,
                                    char *strContextName);
        void ExitContext();
        ContextObject *GetContext();
        BasicErrors *GetErrors();
        BasicStatements *GetStatements();
    private:
                                // Current context we're in
        ContextObject *m_pCurrentContext;
                                // BasicErrors object associated with context
        BasicErrors *m_pBasicErrors;
                                // Collection of BasicStatement objects used to
                                //  compile the Basic source code modules.
        BasicStatements *m_pBasicStatements;
};

#endif

/* end of BasicContext.h */