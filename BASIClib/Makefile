# Makefile entry.
#
#  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.

all:

about:
	cat ../docs/about.txt

clean : 
	rm -f *core
	rm -f *.o
	rm -f *.exe
	rm -f *.dll
	rm -f *.so*
	rm -f *.a

linux : Makefile.linux
	make -f Makefile.linux all

win32 : Makefile.win32
	make -f Makefile.win32 all

# end of Makefile ...

