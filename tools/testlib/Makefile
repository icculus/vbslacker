# Makefile for building a dynamic (shared) BASIClib

export MAJORVER = 0
export MINORVER = 0
export REVISIONVER = 1
export WHOLEVERSION = $(MAJORVER).$(MINORVER).$(REVISIONVER)

export BASICLIBDIR = ../../BASIClib
export GCLIBDIR = ../../libgc

export OBJS = TestLib.o TestConversionFunctions.o TestOnError.o TestThreads.o \
              TestErrorFunctions.o TestStringFunctions.o TestMathFunctions.o \
              TestGarbageCollector.o TestFileSystemFunctions.o \
              TestRandomFunctions.o

export EXES = testconversionfunctions testonerror testthreads \
              testerrorfunctions teststringfunctions testmathfunctions \
              testgarbagecollector testfilesystemfunctions testconsole \
              testsegfault teststackoverflow testrandomfunctions

about:
	@cat ../../docs/about.txt

clean : 
	rm -f core
	rm -f testlib testconsole
	rm -f testlib.exe testconsole.exe
	rm -f TestConsole.o
	rm -f $(OBJS)
	rm -f $(EXES)


linux : Makefile.linux
	@$(MAKE) -f Makefile.linux all

linuxstandalone : Makefile.linux
	@$(MAKE) -f Makefile.linux standalone

win32 : Makefile.win32
	@$(MAKE) -f Makefile.win32 all

# end of Makefile ...
