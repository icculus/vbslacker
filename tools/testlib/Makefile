# Makefile for building a dynamic (shared) BASIClib

MAJORVER = 0
MINORVER = 0
REVISIONVER = 1
WHOLEVERSION = $(MAJORVER).$(MINORVER).$(REVISIONVER)

BASICLIBDIR = ../../BASIClib

OBJS = TestLib.o TestConversionFunctions.o TestOnEvents.o TestThreads.o

CC = gcc
LINKER = gcc
ASM = nasm

# Shipping command lines
#COPTIONS = -D_REENTRANT -Wall -fasm -finline-functions -I $(BASICLIBDIR) -s -O2 -c -o
#LOPTIONS = -Wall -L $(BASICLIBDIR) -lBASIC -lm -lpthread -s -O2 -o
#ASMOPTIONS = -f elf -o

# Debug command lines...
COPTIONS = -D_REENTRANT -Wall -fasm -finline-functions -I $(BASICLIBDIR) -DDEBUG -g -c -o
LOPTIONS = -Wall -L $(BASICLIBDIR) -lBASIC -lm -lpthread -g -o
ASMOPTIONS = -f elf -o

all : linux

linux : testlib

testlib : $(OBJS)
	$(LINKER) $(LOPTIONS) testlib $(OBJS)

about:
	cat ../docs/about.txt

clean : 
	rm -f core
	rm -f testlib$(EXE)
	rm -f $(OBJS)

TestLib.o : TestLib.c TestLib.h
	$(CC) $(COPTIONS) TestLib.o TestLib.c

TestOnEvents.o : TestOnEvents.c $(BASICLIBDIR)/OnEvents.h \
                 $(BASICLIBDIR)/Boolean.h $(BASICLIBDIR)/Threads.h \
                 $(BASICLIBDIR)/ErrorFunctions.h
	$(CC) $(COPTIONS) TestOnEvents.o TestOnEvents.c


TestThreads.o : TestThreads.c 
                $(BASICLIBDIR)/Boolean.h \
                $(BASICLIBDIR)/Threads.h 
	$(CC) $(COPTIONS) TestThreads.o TestThreads.c

TestConversionFunctions.o : TestConversionFunctions.c \
                            $(BASICLIBDIR)/BasicString.h \
                            $(BASICLIBDIR)/ConversionFunctions.h 
	$(CC) $(COPTIONS) TestConversionFunctions.o TestConversionFunctions.c

# end of Makefile ...
