# Useful directories

MYCODEDIR := .

# Directories to search for header files

SEARCHDIRS := -I- -I${MYCODEDIR}

# makemake variables

LINKER       := gcc
DEPENDFLAGS  := -DLINUXELF_I386 -g -Wall -Werror ${SEARCHDIRS}
TOUCHHEADERS := ${MYCODEDIR}/*.h

# make variables

CC       := gcc
CXX      := g++
CCC      := g++
CPPFLAGS  := ${DEPENDFLAGS}
LOADLIBES := -lstdc++

# This is what makemake added


# debug

debug : AssignStatement.o BasicCompiler.o BasicContext.o BasicError.o BasicErrors.o BasicStatement.o BasicStatements.o ContextObject.o Main.o
	${LINKER} ${LDFLAGS} -o $@ ${filter-out %.a, $^} ${LOADLIBES}


# release

release : AssignStatement.o BasicCompiler.o BasicContext.o BasicError.o BasicErrors.o BasicStatement.o BasicStatements.o ContextObject.o Main.o
	${LINKER} ${LDFLAGS} -o $@ ${filter-out %.a, $^} ${LOADLIBES}


# target for making everything

.PHONY : all
all: debug release


# target for removing all object files

.PHONY : tidy
tidy::
	@${RM} core AssignStatement.o BasicCompiler.o BasicContext.o BasicError.o BasicErrors.o BasicStatement.o BasicStatements.o ContextObject.o Main.o

# target for removing all object files

.PHONY : clean
clean:: tidy
	@${RM} debug release

# list of all source files

MM_ALL_SOURCES := AssignStatement.cc BasicCompiler.cc BasicContext.cc BasicError.cc BasicErrors.cc BasicStatement.cc BasicStatements.cc ContextObject.cc Main.cc


# target for checking a source file

CHECKSYNTAXFILE := ${basename ${filter %${CHECKSTRING}, ${MM_ALL_SOURCES}}}

.PHONY : checksyntax
checksyntax:
  ifneq (${CHECKSYNTAXFILE},)
	@${MAKE} ${addsuffix .o, ${CHECKSYNTAXFILE}}
  else
	@echo No target to make ${CHECKSTRING}
  endif


# target for touching appropriate source files

.PHONY : touch
touch:
	@echo
	@echo Please ignore \"file arguments missing\" errors
	@echo
	@echo   `grep -l ${TOUCHSTRING} ${MM_ALL_SOURCES}`
	@-touch `grep -l ${TOUCHSTRING} ${MM_ALL_SOURCES}`
	@echo
	@echo   `grep -l ${TOUCHSTRING} ${TOUCHHEADERS}`
	@-touch `grep -l ${TOUCHSTRING} ${TOUCHHEADERS}`


# target for calculating dependencies

.PHONY : jdepend
jdepend:
	@makemake -depend Makefile -- ${DEPENDFLAGS} -- AssignStatement.cc AssignStatement.o BasicCompiler.cc BasicCompiler.o BasicContext.cc BasicContext.o BasicError.cc BasicError.o BasicErrors.cc BasicErrors.o BasicStatement.cc BasicStatement.o BasicStatements.cc BasicStatements.o ContextObject.cc ContextObject.o Main.cc Main.o


# DO NOT DELETE THIS LINE -- makemake depends on it.

AssignStatement.o: ./AssignStatement.h ./BasicContext.h ./BasicError.h ./BasicErrors.h ./BasicStatement.h ./BasicStatements.h ./ContextObject.h ./boolean.h /usr/include/stdlib.h

BasicCompiler.o: ./BasicCompiler.h ./BasicContext.h ./BasicError.h ./BasicErrors.h ./BasicStatement.h ./BasicStatements.h ./ContextObject.h ./boolean.h /usr/include/stdio.h /usr/include/stdlib.h /usr/include/string.h

BasicContext.o: ./BasicContext.h ./BasicError.h ./BasicErrors.h ./BasicStatement.h ./BasicStatements.h ./ContextObject.h ./boolean.h /usr/include/stdlib.h

BasicError.o: ./BasicError.h ./ContextObject.h

BasicErrors.o: ./BasicContext.h ./BasicError.h ./BasicErrors.h ./BasicStatement.h ./BasicStatements.h ./ContextObject.h ./boolean.h /usr/include/stdlib.h

BasicStatement.o: ./BasicContext.h ./BasicError.h ./BasicErrors.h ./BasicStatement.h ./BasicStatements.h ./ContextObject.h ./boolean.h /usr/include/stdlib.h

BasicStatements.o: ./AssignStatement.h ./BasicContext.h ./BasicError.h ./BasicErrors.h ./BasicStatement.h ./BasicStatements.h ./ContextObject.h ./boolean.h /usr/include/stdlib.h

ContextObject.o: ./ContextObject.h /usr/include/string.h

Main.o: ./BasicCompiler.h ./BasicContext.h ./BasicError.h ./BasicErrors.h ./BasicStatement.h ./BasicStatements.h ./ContextObject.h ./boolean.h /usr/include/stdio.h /usr/include/stdlib.h

