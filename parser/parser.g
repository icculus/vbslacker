/* parser.g - Grammar for parsing tokens retrieved by the lexer
 *
 * Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

header
{
    #include "PostParser.h"
}

options
{
    language = "Cpp";       // We want to generate C++ output
}

class BasicParser extends Parser;
options
{
    // Import tokens from the lexer
    importVocab = Basic;

    // Turn off default error handling
    defaultErrorHandler=false;
}

{
private:
    // Here's the post parser that we'll be passing parsed data to.
    PostParser *pPostParser;
}

start[PostParser *pNewPostParser]:
/* Description
 *      This function is called to start the parsing
 * Parameters
 *      none
 * Returns
 *      none
 */
    {
        // Save pointer to the parser being passed to our start method.
        pPostParser = pNewPostParser;
    }
    // Continue parsing until we receive an EOF
    //(statement NEWLINE)*
    //EOF

    /* Since we're getting errors when parsing multiple lines in the parser
     * grammar, we're parsing only a single statement.  See the main.cpp for
     * reasons why we can't parse using the above statements.
     */
    statement (NEWLINE)?
    ;
    exception
    catch [ParserException ex]
    {
        pPostParser->ReportError(&ex);
    }

statement:
/* Description
 *      We come here each time we need to start parsing a single vb statement.
 * Parameters
 *      none
 * Returns
 *      none
 */
    (dim_statement
    // We'll get a proc_declaration if no scope identifier preceded the
    // declaration.  We shall assume public then.
    | proc_declaration[SCOPE_PUBLIC]
    | scope_statement
    | identifier_statement
    | end_statement
    | for_statement
    | next_statement
    | print_statement)?
    (COMMENT)?
    ;

scope_statement
/* Description
 *      We get here if the statement starts with a scope identifier.  From here
 *      we will test it for a variable declaration, or a procedure declaration.
 *      We also will pass the scope identifier that was specified along with
 *      the call.
 * Parameters
 *      none
 * Returns
 *      none
 */
    {
        BasicScope scope;
    }:

    (PUBLIC
    {
        scope = SCOPE_PUBLIC;
    }
    | PRIVATE
    {
        scope = SCOPE_PRIVATE;
    }
    | FRIEND
    {
        scope = SCOPE_FRIEND;
    })

    (var_declaration[scope]
    | proc_declaration[scope])
    ;

dim_statement:
/* Description
 *      We'll get here if we've hit any statement with a DIM keyword
 * Parameters
 *      none
 * Returns
 *      none
 */
    // Immediately spawn to the variable declaration method.  The SCOPE_DIM
    //  can indicate a different scope depending on what the context of
    //  the DIM statement is.
    DIM var_declaration[SCOPE_DIM]
    ;

proc_declaration[BasicScope Scope]:
/* Description
 *      We'll either get here as a root statement, or spawn to here from the
 *      'scope_statement' function.
 * Parameters
 *      Scope   -   Specifies the scope of the procedure declaration
 * Returns
 *      none
 */
    (STATIC)? (SUB IDENTIFIER decl_arglist)
    | (FUNCTION IDENTIFIER decl_arglist (AS IDENTIFIER)?)
    ;

identifier_statement:
/* Description
 *      We'll get here on any statement that starts with an identifier.
 *      From here we decide whether it's an assignment statement, or
 *      a procedure call.
 * Parameters
 *      none
 * Returns
 *      none
 */
    id:IDENTIFIER
    (assignment_statement[id] | procedure_call[id])
    ;

for_statement:
/* Description
 *      This method processes a FOR statement
 * Parameters
 *      none
 * Returns
 *      none
 */
    FOR
    (IDENTIFIER EQUAL expression TO expression (STEP expression)?)
    | (EACH IDENTIFIER IN IDENTIFIER)
    ;

next_statement:
/* Description
 *      Can't have a for statement without a next statement, eh?
 * Parameters
 *      none
 * Returns
 *      none
 */
    NEXT (IDENTIFIER)?
    ;

end_statement:
/* Description
 *      Any statement beginning with 'end' will start here
 * Parameters
 *      none
 * Returns
 *      none
 */
    END
    (SUB | FUNCTION | IF | PROPERTY | SELECT | TYPE | WITH)?
    ;

print_statement:
/* Description
 *      The totally ambiguous and sucky print statement
 * Parameters
 *      none
 * Returns
 *      none
 */
    PRINT (POUND NUMBER COMMA)?
    ( (SPC_FUNC LPAREN expression RPAREN)
    | (TAB_FUNC LPAREN expression RPAREN)
    | COMMA
    | SEMICOLON
    | expression)+
    ;

/* Branch statements */
decl_arglist:
/* Description
 *      This defines a set of one or more arguments in a procedure declaration.
 * Parameters
 *      none
 * Returns
 *      none
 */
    LPAREN
    (decl_arg (COMMA decl_arg)*)?
    RPAREN
    ;

decl_arg:
/* Description
 *      This is the specification for a single argument in a procedure
 *      declaration.
 * Parameters
 *      none
 * Returns
 *      none
 */
    (OPTIONAL)? (BYVAL | BYREF)? (PARAMARRAY)? IDENTIFIER
    (LPAREN RPAREN)? AS IDENTIFIER (EQUAL expression)?
    ;

var_declaration[BasicScope Scope]
/* Description
 *      We come here when one or more variables are being declared...either
 *      using a scope identifier, or with the DIM statement.
 * Parameters
 *      none
 * Returns
 *      none
 */
    {
        // Initialize
        RefToken rtVarName;
        RefToken rtDataType;
        Boolean blnWithEvents;
        Boolean blnAsNew;
    }:
    var_decl[rtVarName, rtDataType, blnWithEvents, blnAsNew]
    {
        // If we get here, we'll have a valid variable declaration
        pPostParser->VarDecl(Scope, rtVarName, rtDataType, blnWithEvents, blnAsNew);
    }
    (COMMA var_declaration[Scope])?
    ;

var_decl[RefToken &VarName, RefToken &DataType, Boolean &WithEvents,
Boolean &AsNew]
/* Description
 *      Represents a single variable declaration only.  All parameters that
 *      are passed are set by this function.
 * Parameters
 *      VarName     -   Token that represents the variable name being declared.
 *      DataType    -   Token that represents the datatype of the variable.
 *      WithEvents  -   Was the variable declared as 'WithEvents'?
 *      AsNew       -   Was the variable instantiated as a new object?
 * Returns
 *      none
 */
    // Initialization before processing variable declaration.
    {
        WithEvents = FALSE;
        AsNew = FALSE;
    }:
    (withevents_token:WITHEVENTS)?
    {
        // If we found a 'WithEvents' token
        if(withevents_token != NULL) WithEvents = TRUE;
    }
    varname_token:IDENTIFIER
    {
        // Retain identifier token
        VarName = varname_token;
    }
    (LPAREN (subscripts)? RPAREN)?
    (
    // "As" or "As New" is acceptable
    ((AS NEW) => (AS NEW)
    {
        // If "As New" was specified
        AsNew = TRUE;
    }
    | AS)

    datatype_token:IDENTIFIER
    {
        // Save our datatype that was specified
        DataType = datatype_token;
    }
    )?
    ;

subscripts:
/* Description
 *      We'll get here if we expect an array subscript range.
 * Parameters
 *      none
 * Returns
 *      none
 */
    (expression TO) => (lower:expression TO upper:expression)
    | upper2:expression (subscripts)?
    ;

assignment_statement[RefToken VarName]:
/* Description
 *      Just a plain old assignment statement.
 * Parameters
 *      VarName - Token that represents the variable name being assigned to.
 * Returns
 *      none
 */
    EQUAL expr:expression
    ;

procedure_call[RefToken FuncName]:
/* Description
 *      Just a plain old procedure call.
 * Parameters
 *      none
 * Returns
 *      none
 */
    (expression (COMMA expression)*)?
    ;

function_call[RefToken FuncName]:
/* Description
 *      Just a plain old function call.  A function call differs from a
 *      procedure call in that the arguments are surrounded by parenthesis.
 * Parameters
 *      none
 * Returns
 *      none
 */
    LPAREN expression (COMMA expression)* RPAREN
    ;

expression:
/* Description
 *      Represents a valid numeric or string expression
 * Parameters
 *      none
 * Returns
 *      none
 */
    (unary_operator atom binary_operator) =>
        (unary_operator atom binary_operator expression)
    | (unary_operator atom)
    | (atom binary_operator) => (atom binary_operator expression)
    | atom
    ;

atom:
/* Description
 *      This is a single non-operator component of an expression.
 * Parameters
 *      none
 * Returns
 *      none
 */
    (IDENTIFIER LPAREN) => (funcname:IDENTIFIER function_call[funcname])
    | IDENTIFIER | NUMBER | STRING | (LPAREN expression RPAREN)
    ;

unary_operator:
/* Description
 *      Any operator that only requires an atom following the operator.
 * Parameters
 *      none
 * Returns
 *      none
 */
    PLUS | MINUS
    ;

binary_operator:
/* Description
 *      Any operator that requires two atoms with the operator, located
 *      before and after it.
 * Parameters
 *      none
 * Returns
 *      none
 */
    PLUS | MINUS | MULT | DIV
    ;