/* This is temporary until I have a better place to put it */
header
{
   /* This enumeration is used for passing the scope of a variable or
    * function declaration.
    */
   enum BasicScope {SCOPE_PUBLIC, SCOPE_PRIVATE, SCOPE_DIM, SCOPE_FRIEND};
}

options {
   language = "Cpp";
}

class BasicParser extends Parser;
options {
   importVocab = Basic;
}

start:
/* Start here
 */
   (statement NEWLINE)*
   EOF
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

var_declaration[BasicScope scope]:
   (WITHEVENTS)? varname:IDENTIFIER (LPAREN (subscripts)?
   RPAREN)? (AS (NEW)? datatype:IDENTIFIER)? (COMMA var_declaration[scope])?
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