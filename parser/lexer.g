options {
   language = "Cpp";             // We want to generate C++ output
}

class BasicLexer extends Lexer;
options {
   // Nothing is case sensitive in VB.
   caseSensitiveLiterals = false;

   exportVocab = Basic;
   k = 2;
}

tokens {
   DIM = "dim";
   PUBLIC = "public";
   PRIVATE = "private";
   SUB = "sub";
   FUNCTION = "function";
   AS = "as";
   BYVAL = "byval";
   BYREF = "byref";
   END = "end";
   FOR = "for";
   NEXT = "next";
   TO = "to";
   STEP = "step";
   IF = "if";
   PROPERTY = "property";
   SELECT = "select";
   TYPE = "type";
   WITHEVENTS = "withevents";
   WITH = "with";
   EACH = "each";
   IN = "in";
   STATIC = "static";
   OPTIONAL = "optional";
   PARAMARRAY = "paramarray";
   SPC_FUNC = "spc";
   TAB_FUNC = "tab";
   PRINT = "print";
}

STRING
   options {
      paraphrase = "string";
   }:
   // We're deliberately skipping the '"' character as a valid character for
   //  a string.  This is not allowed in VB, and it would confuse the parser
   //  anyway.
   '\"' (' '..'!' | '#'..'~')* '\"';

NUMBER: ('0'..'9')+ ('.' ('0'..'9')+)?;

COMMENT: '\'' (~'\n')* '\n'
   {
      _ttype = Token::SKIP;
   } ;

WHITESPACE: ('\t' | ' ')
   { _ttype = Token::SKIP; };

NEWLINE
   options {
      paraphrase = "newline";
   }: ('\n' | '\r' | ('\n''\r'))
   {
      newline();
   };

// Operators
PLUS :'+';

MINUS : '-';

MULT : '*';

DIV : '/';

LPAREN
   options {
      paraphrase = "'('";
   }:
   '(';

RPAREN
   options {
      paraphrase = "')'";
   }:
   ')';

COMMA
   options {
      paraphrase = "','";
   }:
   ',';

EQUAL
   options {
      paraphrase = "'='";
   }:
   '=';

SEMICOLON
   options {
      paraphrase = "';'";
   }:
   ';';

POUND
   options {
      paraphrase = "'#'";
   }:
   '#';

IDENTIFIER
   options {
      paraphrase = "identifier";
   }:

   ('a'..'z' | 'A'..'Z') ('a'..'z' | 'A'..'Z' | '0'..'9')*;