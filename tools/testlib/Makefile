# Makefile for building a dynamic (shared) BASIClib

MAJORVER = 0
MINORVER = 0
REVISIONVER = 1
WHOLEVERSION = $(MAJORVER).$(MINORVER).$(REVISIONVER)

BASICLIBDIR = ../../BASIClib

about:
	cat ../../docs/about.txt

clean : 
	rm -f core
	rm -f *.exe
	rm -f testlib
	rm -f $(OBJS)

linux : Makefile.linux
	make -f Makefile.linux all

win32 : Makefile.win32
	make -f Makefile.win32 all

# end of Makefile ...
