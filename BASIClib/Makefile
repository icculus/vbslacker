# Makefile entry.
#
#  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.


export MAJORVER = 0
export MINORVER = 0
export REVISIONVER = 1
export WHOLEVERSION = $(MAJORVER).$(MINORVER).$(REVISIONVER)
export OBJS = BasicString.o ConversionFunctions.o Initialize.o \
              ErrorFunctions.o MathFunctions.o MemoryManager.o Threads.o \
              TimeDateFunctions.o BasicFileSystem.o \
              StringFunctions.o LowLevelFunctions.o SignalHandlers.o \
              ConsoleFunctions.o ProcessFunctions.o FileSystemFunctions.o \
              Variant.o BasicError.o NoConsole.o DirectConsole.o \
              RedirectedConsole.o CursesConsole.o EnvrFunctions.o Gosub.o \
              MiscFunctions.o ArrayFunctions.o GUIFrontEnd.o \
              RegistryFunctions.o Language.o DynamicLoader.o


about:
	@cat ../docs/about.txt

lines:
	@linecount $(wildcard *.c) $(wildcard *.h) $(wildcard Makefile*)

clean:
	rm -f $(wildcard *core)
	rm -f $(wildcard *.o)
	rm -f $(wildcard *.exe)
	rm -f $(wildcard *.dll)
	rm -f $(wildcard *.so*)
	rm -f $(wildcard *.a)

linux: Makefile.linux
	@$(MAKE) -f Makefile.linux all

win32: Makefile.win32
	@$(MAKE) -f Makefile.win32 all

# end of Makefile ...

