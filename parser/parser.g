/* This is temporary until I have a better place to put it */
header
{
   #include "PostParser.h"
}

options {
   language = "Cpp";
}

// Heading of BasicParser.cpp
{
}

class BasicParser extends Parser;
options {
   importVocab = Basic;
}

{
   PostParser *pPostParser;
}

start:
// Start here
   {
      pPostParser = new PostParser;
   }
   (statement NEWLINE)*
   EOF
   {
      delete pPostParser;
   }
   ;

statement:
   (dim_statement
   | proc_declaration[SCOPE_PUBLIC]    // Assume public if not specified
   | scope_statement
   | identifier_statement
   | end_statement
   | for_statement
   | next_statement
   | print_statement)?
   (COMMENT)?
   ;

/* Root statements */
scope_statement
/* If statement starts with a scope (i.e. public, private, friend) then
 * we'll start here and branch off to a variable declaration or a
 * procedure declaration
 */
   {
      BasicScope scope;
   }:

   (PUBLIC {scope = SCOPE_PUBLIC;}
   | PRIVATE {scope = SCOPE_PRIVATE;}
   | FRIEND {scope = SCOPE_FRIEND;})
   
   (var_declaration[scope]
   | proc_declaration[scope])
   ;

dim_statement:
/* If DIM is the starting keyword, we'll expect a regular variable declaration
 * to proceed it.
 */
   DIM var_declaration[SCOPE_DIM]
   ;

proc_declaration[BasicScope scope]:
/* We'll either get here as a root statement, or spawn to here from the
 * 'scope_statement' function.
 */
   (STATIC)? 
   (SUB IDENTIFIER decl_arglist)
   | (FUNCTION IDENTIFIER decl_arglist (AS IDENTIFIER)?)
   ;

identifier_statement:
/* We get here on any statement that starts with an identifier.
 */
   id:IDENTIFIER
   (assignment_statement["blagh"] | procedure_call["blagh"])
   ;

for_statement:
/* Just a for statement...yeah.
 */
   FOR
   (IDENTIFIER EQUAL expression TO expression (STEP expression)?)
   | (EACH IDENTIFIER IN IDENTIFIER)
   ;

next_statement:
/* Can't have a for statement without a next statement, eh?
 */
   NEXT (IDENTIFIER)?
   ;

end_statement:
/* Any statement beginning with 'end' will start here
 */
   END
   (SUB 
   | FUNCTION
   | IF
   | PROPERTY
   | SELECT
   | TYPE
   | WITH)?
   ;

print_statement:
/* The totally ambiguous and sucky print statement
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
/* This defines a set of one or more arguments in a procedure declaration.
 */
   LPAREN
   (decl_arg (COMMA decl_arg)*)?
   RPAREN
   ;

decl_arg:
/* This is the specification for a single argument in a procedure declaration.
 */
   (OPTIONAL)? (BYVAL | BYREF)? (PARAMARRAY)? IDENTIFIER
   (LPAREN RPAREN)? AS IDENTIFIER (EQUAL expression)?
   ;

// One or more variable declarations
var_declaration[BasicScope Scope]
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

// Represents a single variable declaration only
var_decl[RefToken &VarName, RefToken &DataType, Boolean &WithEvents,
Boolean &AsNew]
   {
      WithEvents = FALSE;
      AsNew = FALSE;
   }:

   (withevents_token:WITHEVENTS)?
      { if(withevents_token != NULL) WithEvents = TRUE; }
   varname_token:IDENTIFIER
      { VarName = varname_token; }
   (LPAREN (subscripts)? RPAREN)? (AS
   (new_token:NEW)?
      { if(new_token != NULL) AsNew = TRUE; }
   datatype_token:IDENTIFIER
      { DataType = datatype_token; }
   )?
   ;
   
subscripts:
   (expression TO) => (lower:expression TO upper:expression)
   | upper2:expression (subscripts)?
   ;

assignment_statement[char *varname]:
   EQUAL expr:expression
   ;

procedure_call[char *funcname]:
   expression (COMMA expression)*
   ;

function_call[char *funcname]:
   LPAREN expression (COMMA expression)* RPAREN
   ;

expression:
   (unary_operator atom binary_operator) => 
      (unary_operator atom binary_operator expression)
   | (unary_operator atom)
   | (atom binary_operator) => (atom binary_operator expression)
   | atom
   ;

atom:
   (IDENTIFIER LPAREN) => (funcname:IDENTIFIER function_call["blagh"])
   | IDENTIFIER | NUMBER | STRING | (LPAREN expression RPAREN)
   ;

unary_operator:
   PLUS | MINUS
   ;

binary_operator:
   PLUS | MINUS | MULT | DIV
   ;