# Makefile entry
#
#  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.

export ANTLR_OBJS = ANTLRException.o AST.o ASTFactory.o BitSet.o CharBuffer.o \
	CharScanner.o CommonASTNode.o CommonASTNodeWithHiddenTokens.o \
	CommonHiddenStreamToken.o CommonToken.o InputBuffer.o \
	LLkParser.o LexerSharedInputState.o MismatchedTokenException.o \
	NoViableAltException.o Parser.o ParserException.o \
	ParserSharedInputState.o ScannerException.o String.o Token.o \
	TokenBuffer.o TokenStreamBasicFilter.o \
	TokenStreamHiddenTokenFilter.o TokenStreamSelector.o \
	TreeParser.o TreeParserSharedInputState.o

# Directories to search for header files
export ANTLR_SEARCHDIRS = -I- -I.

# Default if no build type specified
default:
	@echo 'No target specified.'

# Build debug version
linuxdebug: Makefile.linux
	@$(MAKE) -f Makefile.linux debug

win32debug: Makefile.win32
	@$(MAKE) -f Makefile.win32 debug

clean:
	rm *.o
	
# end of Makefile ...
