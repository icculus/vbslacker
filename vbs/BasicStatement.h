/*
 *  Declarations for the BasicStatement class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICSTATEMENT_H_
#define _INCLUDE_BASICSTATEMENT_H_

#include "BasicContext.h"


class BasicStatement
{
    public:
        void BasicStatement(BasicContext *pContext);
        void ~BasicStatement();

        void ProcessStatement(char *strStatement);
    private:
        char *m_strToken;
        BasicContext *m_pContext;
};

#endif

/* end of BasicStatement.h */