/*
 * Declarations for the ContextObject class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_CONTEXTOBJECT_H_
#define _INCLUDE_CONTEXTOBJECT_H_

// Context type data type
enum CONTEXT_TYPE {
    CONTEXT_PROJECT                 =   0x00,
    CONTEXT_MODULE_BAS              =   0x10,
    CONTEXT_MODULE_CLS              =   0x11,
    CONTEXT_MODULE_FRM              =   0x12,
    CONTEXT_PROCEDURE_SUB           =   0x20,
    CONTEXT_PROCEDURE_FUNCTION      =   0x21,
    CONTEXT_PROCEDURE_PROPERTY_GET  =   0x22,
    CONTEXT_PROCEDURE_PROPERTY_LET  =   0x23,
    CONTEXT_PROCEDURE_PROPERTY_SET  =   0x24,
    CONTEXT_TYPE_ENUM               =   0x30,
    CONTEXT_TYPE_USERDEFINED        =   0x31
};

// Interface
class ContextObject
{
    public:
        void ContextObject(CONTEXT_TYPE ContextType, char *strContextName, 
                           ContextObject *pParentContext);
        void ~ContextObject();

        ContextObject *GetParent(); 
        CONTEXT_TYPE GetType();
        char *GetName();
    private:
        char *m_strContextName; // Name of context object.
                                
                                // Type of context
        CONTEXT_TYPE m_ContextType;
                                // Pointer to parent context object.
        ContextObject *m_pParentContext;
};

#endif

/* end of ContextObject.h */