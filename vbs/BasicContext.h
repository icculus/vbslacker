/*
 * Declarations for the ContextObject class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICCONTEXT_H_
#define _INCLUDE_BASICCONTEXT_H_

#include "BasicErrors.h"
#include "BasicStatements.h"
#include "ContextObject.h"
#include "Boolean.h"
#include <stdlib.h>

enum BASIC_EVENT {
    BASIC_EVENT_ERROR           =   0
};
typedef void (*BASIC_EVENT_HANDLER)(BASIC_EVENT, void *);

class BasicContext
{
    public:
        BasicContext(BASIC_EVENT_HANDLER pEventHandler = NULL);
        ~BasicContext();

        ContextObject *EnterContext(CONTEXT_TYPE Context,
                                    char *strContextName);
        void ExitContext();
        ContextObject *GetContext();
        BasicErrors *GetErrors();
        BasicStatements *GetStatements();
        void SetTerminateFlag();
        BOOLEAN GetTerminateFlag();
        void RaiseEvent(BASIC_EVENT Event, void *pData = NULL);
    private:
                                // Current context we're in
        ContextObject *m_pCurrentContext;
                                // BasicErrors object associated with context
        BasicErrors *m_pBasicErrors;
                                // Collection of BasicStatement objects used to
                                //  compile the Basic source code modules.
        BasicStatements *m_pBasicStatements;
                                // Array of ContextObjects that we created.
        ContextObject **m_pContextObjects;
                                // Pointer to an optional event handler
        BASIC_EVENT_HANDLER m_pEventHandler;
                                // Number of ContextObject instances that were
                                //  created by this BasicContext instance.
        short m_NumberOfContextObjects;
                                // TRUE if compilation has been requested to
                                //  terminate.
        BOOLEAN m_TerminateFlag;
};

#endif

/* end of BasicContext.h */