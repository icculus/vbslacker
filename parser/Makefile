# Makefile entry.
#
#  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.


# Useful directories
export ANTLR_BASE = $(HOME)/antlr-2.6.1
export ANTLR_CPP = $(ANTLR_BASE)/cpp
export OBJS = BasicLexer.o BasicParser.o main.o
export BUILD_FOLDER = `pwd`

export ANTLR_OBJS = $(ANTLR_CPP)/*.o

export ANTLR = java antlr.Tool

# Directories to search for header files
export SEARCHDIRS = -I- -I. -I$(ANTLR_CPP)

default:
	@echo "No target specified."

linuxdebug: Makefile.linux
	@$(MAKE) -f Makefile.linux debug

win32debug: Makefile.win32
	@$(MAKE) -f Makefile.win32 debug

clean:
	rm *.o
	rm BasicParser.cpp BasicParser.hpp
	rm BasicLexer.cpp BasicLexer.hpp
	rm BasicParserTokenTypes.hpp BasicParserTokenTypes.txt
	rm BasicTokenTypes.hpp BasicTokenTypes.txt
