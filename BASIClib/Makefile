# Makefile entry.
#
#  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.



# Makefile for building a static, win32 BASIClib  !!! For now.

export MAJORVER = 0
export MINORVER = 0
export REVISIONVER = 1
export WHOLEVERSION = $(MAJORVER).$(MINORVER).$(REVISIONVER)


about:
	@cat ../docs/about.txt

clean : 
	rm -f $(wildcard *core)
	rm -f $(wildcard *.o)
	rm -f $(wildcard *.exe)
	rm -f $(wildcard *.dll)
	rm -f $(wildcard *.so*)
	rm -f $(wildcard *.a)

linux : Makefile.linux
	@$(MAKE) -f Makefile.linux all

win32 : Makefile.win32
	@$(MAKE) -f Makefile.win32 all

# end of Makefile ...

