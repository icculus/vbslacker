# Makefile for building a dynamic (shared) BASIClib

export MAJORVER = 0
export MINORVER = 0
export REVISIONVER = 1
export WHOLEVERSION = $(MAJORVER).$(MINORVER).$(REVISIONVER)

export BASICLIBDIR = ../../BASIClib

about:
	@cat ../../docs/about.txt

clean : 
	rm -f core
	rm -f $(wildcard *.exe)
	rm -f testlib
	rm -f $(wildcard *.o)

linux : Makefile.linux
	@$(MAKE) -f Makefile.linux all

win32 : Makefile.win32
	@$(MAKE) -f Makefile.win32 all

# end of Makefile ...
