# Makefile for building a dynamic (shared) BASIClib

MAJORVER = 0
MINORVER = 0
REVISIONVER = 1
WHOLEVERSION = $(MAJORVER).$(MINORVER).$(REVISIONVER)

DYNLIBBASE = libBASIC.so
DYNLIBWHOLE = $(DYNLIBBASE).$(WHOLEVERSION)
DYNLIBMAJOR = $(DYNLIBBASE).$(MAJORVER)

OBJS = BasicString.o ConversionFunctions.o Initialize.o OnEvents.o \
       ErrorFunctions.o MathFunctions.o InternalMemManager.o Threads.o \
       TimeDateFunctions.o OnEventsAsm.o

CC = gcc
LINKER = gcc
ASM = nasm

# Shipping command lines
#COPTIONS = -Wall -fPIC -fasm -finline-functions -O2 -s -c -o
#LOPTIONS = -shared -Wl,-soname,$(DYNLIBLN) -s -O2 -o

# Debug command lines...
COPTIONS = -Wall -fPIC -fasm -finline-functions -DDEBUG -g -c -o
LOPTIONS = -shared -Wl,-soname,$(DYNLIBLN) -g -o
ASMOPTIONS = -f elf -o

# end of makelib ...

all : $(DYNLIBBASE)

$(DYNLIBBASE) : $(DYNLIBMAJOR)
	ln -sf $(DYNLIBMAJOR) $(DYNLIBBASE)

$(DYNLIBMAJOR) : $(DYNLIBWHOLE)
	ln -sf $(DYNLIBWHOLE) $(DYNLIBMAJOR)

$(DYNLIBWHOLE) : $(OBJS)
	$(LINKER) $(LOPTIONS) $(DYNLIBWHOLE) $(OBJS)

about:
	cat ../docs/about.txt

clean : 
	rm -f core
	rm -f $(DYNLIBBASE)
	rm -f $(DYNLIBMAJOR)
	rm -f $(DYNLIBWHOLE)
	rm -f $(OBJS)

BasicString.o : BasicString.c BasicString.h ErrorFunctions.h Boolean.h \
                MathFunctions.h InternalMemManager.h
	$(CC) $(COPTIONS) BasicString.o BasicString.c

ConversionFunctions.o : ConversionFunctions.c ConversionFunctions.h Boolean.h \
                        InternalMemManager.h MathFunctions.h BasicString.h \
                        ErrorFunctions.h Variant.h
	$(CC) $(COPTIONS) ConversionFunctions.o ConversionFunctions.c

Initialize.o : Initialize.c Initialize.h Threads.h OnEvents.h \
               TimeDateFunctions.h
	$(CC) $(COPTIONS) Initialize.o Initialize.c

OnEvents.o : OnEvents.c OnEvents.h InternalMemManager.h Threads.h
	$(CC) $(COPTIONS) OnEvents.o OnEvents.c

ErrorFunctions.o : ErrorFunctions.c ErrorFunctions.h OnEvents.h Threads.h
	$(CC) $(COPTIONS) ErrorFunctions.o ErrorFunctions.c

MathFunctions.o : MathFunctions.c MathFunctions.h
	$(CC) $(COPTIONS) MathFunctions.o MathFunctions.c

InternalMemManager.o : InternalMemManager.c InternalMemManager.h \
                       ErrorFunctions.h
	$(CC) $(COPTIONS) InternalMemManager.o InternalMemManager.c

Threads.o : Threads.c Threads.h
	$(CC) $(COPTIONS) Threads.o Threads.c

TimeDateFunctions.o : TimeDateFunctions.c TimeDateFunctions.h BasicString.h \
                      ErrorFunctions.h Boolean.h
	$(CC) $(COPTIONS) TimeDateFunctions.o TimeDateFunctions.c

OnEventsAsm.o : OnEvents.asm
	$(ASM) $(ASMOPTIONS) OnEventsAsm.o OnEvents.asm

# end of Makefile ...

