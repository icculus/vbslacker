-Declarations MUST appear before functions
-Everything is a statement/function

Class Structure
---------------
#define CONTEXT_TYPE short

// Context Types
#define CONTEXT_PROJECT_EXE 0
#define CONTEXT_PROJECT_DLL 1
#define CONTEXT_PROJECT_ACTIVEX_CONTROL 2
#define CONTEXT_PROJECT_ACTIVEX_EXE 3
#define CONTEXT_PROJECT_ACTIVEX_DLL 4
#define CONTEXT_MODULE_BAS 5
#define CONTEXT_MODULE_CLS 6
#define CONTEXT_MODULE_FRM 7

class ContextObject
{
    public:
       void ContextObject(CONTEXT_TYPE ContextType, char *strContextName);
       void ~ContextObject();

       ContextObject *GetParent();
       CONTEXT_TYPE GetType();
       char *GetName();
    private:
       char *m_strContextName;
       CONTEXT_TYPE m_ContextType;
};

class BasicContext
{
    public:
       void BasicContext();
       void ~BasicContext();

       ContextObject *EnterContext(CONTEXT_TYPE ContextType, char *strContextName);
       BOOL ExitContext(ContextObject *pContextObject);
       ContextObject *GetContext();
    private:
};

class BasicStatement
{
    public:
       void BasicStatement(BasicContext *pContext);
       void ~BasicStatement();

       short StatementType(char *strStatement);
    private:
       char *strToken;
 
       COMPILE_ERROR_TYPE Compile()=0;
};

class AssignmentStatement : public BasicStatement
{
};
   

Tokens
   * = single token (name, not an operator or symbol)

   (VB statements)
   If
   For
   Open
   Input
   Line Input
   Print
   Write
   End
   
   (Assignment Statement)
   Set
   Let
   * =
   
   (Function Declaration)
   Private Sub
   Private Function
   Private Static Sub
   Private Static Function
   Public Sub
   Public Function
   Public Static Sub
   Public Static Function
   Friend Sub
   Friend Function
   Friend Static Sub
   Friend Static Function

   (Variable Declaration)
   Public
   Dim
   Private
   Static

   (Constant Declaration)
   Public Const
   Private Const
   Const   

   (Function Call)
   Call
   *


***In search order***
if statement

for statement

assignment statement
   Tokens
      [Set, Let] <varname> =

   [Set,Let] <varname> [=] <expression>
function call statement
   Tokens
      Call <procedure>
      <procedure> !=!

   [Call] <procedure>[(<arglist>)]

   If "Call" is used, the arglist MUST be surrounded by "()" and the return value is discarded.

   If return value is used, arglist must be surrounded by "()".

variable declaration
   Tokens
      {Public, Dim, Private, Static} [WithEvents] <varname>

   BAS module
      {Public,Dim,Private} [WithEvents] <varname>[([<subscripts>])] [As [New] <type>] [,[WithEvents] <varname>[([<subscripts>])] [As [New] <type>]]...
   CLS module
      {Public,Dim,Private} [WithEvents] <varname>[([<subscripts>])] [As [New] <type>] [,[WithEvents] <varname>[([<subscripts>])] [As [New] <type>]]...
   procedural
      {Dim,Static} <varname>[([<subscripts>])] [As [New] <type>] [, <varname>[([<subscripts>])] [As [New] <type>]]...
constant declaration
   Tokens
      [Public, Private] Const

   BAS module
      [Public, Private] Const <constname> [As <type>] = <expression>
   CLS module
      [Private] Const <constname> [As <type>] = <expression>
   procedural
      Const <constname> [As <type>] = <expression>
function declaration
   Tokens
      [Private, Public, Friend] [Static] {Sub, Function}

   arg = [Optional] [ByVal | ByRef] [ParamArray] varname[( )] [As type] [= defaultvalue]

   BAS module
   [Private, Public] [Static] Sub <name> [(arg[, ...])]
   [End Sub]

   [Private, Public] [Static] Function <name> [(arg[, ...])] [As <type>]
   [End Sub]

   CLS module
   [Private, Public, Friend] [Static] Sub <name> [(arg[, ...])]
   [End Sub]

   [Private, Public, Friend] [Static] Function <name> [(arg[, ...])] [As <type>]
   [End Sub]
End Sub
