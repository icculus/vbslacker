#----------------------------------------------------------------------------#
# Defaults for building BASIClib. Please edit for your system.
#
#  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
#----------------------------------------------------------------------------#

#----------------------------------------------------------------------------#
# Version of the library.
#----------------------------------------------------------------------------#
MAJORVER = 0
MINORVER = 0
REVISIONVER = 1
#----------------------------------------------------------------------------#

#----------------------------------------------------------------------------#
# Where you want the shared libraries installed.
#----------------------------------------------------------------------------#
INSTPREFIX=/usr/local/lib
#----------------------------------------------------------------------------#

#----------------------------------------------------------------------------#
# Set to 'true' if you want to enable debugging.
#----------------------------------------------------------------------------#
debug=true
#----------------------------------------------------------------------------#

#----------------------------------------------------------------------------#
# Set to 'true' if you want a single-threaded BASIClib.
#----------------------------------------------------------------------------#
singlethreaded=true
#----------------------------------------------------------------------------#

#----------------------------------------------------------------------------#
# Compilers for your system. (Probably should leave this alone.)
#----------------------------------------------------------------------------#
CC = gcc
LINKER = gcc
#----------------------------------------------------------------------------#

#----------------------------------------------------------------------------#
# Directory of garbage collector library. (Probably should leave this alone.)
#----------------------------------------------------------------------------#
GCDIR = ../libgc
#----------------------------------------------------------------------------#

#----------------------------------------------------------------------------#
# Name of garbage collector binary. (Probably should leave this alone.)
#----------------------------------------------------------------------------#
GCLIB = $(GCDIR)/gc.a
#----------------------------------------------------------------------------#

#----------------------------------------------------------------------------#
# Don't touch anything below this line...
#----------------------------------------------------------------------------#

WHOLEVERSION = $(MAJORVER).$(MINORVER).$(REVISIONVER)

OS = $(shell uname -s)
ARCH = $(shell uname -m)

ifeq ($(OS), Linux)
  ifeq ($(ARCH), ppc)
    COPTIONS += -DLINUXELF_PPC
  else
    COPTIONS += -DLINUXELF_I386
  endif
endif

ifeq ($(OS), win32)
  COPTIONS += -DWIN32_I386
endif

# !!! I dare you to put -pendantic in here. and -Werror and -ansi
WARNS = -Wall -W -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast
WARNS += -Wcast-align -Wwrite-strings -Waggregate-return -Wredundant-decls
WARNS += -Wnested-externs -Winline -Wold-style-cast -Wstrict-prototypes
#WARNS += -Wmissing-declarations -Wmissing-prototypes -Wconversion

DEFS = -D_REENTRANT -DLINUX_THREADS

ifeq ($(singlethreaded), true)
  DEFS += -DSINGLE_THREADED
endif

ifeq ($(debug), true)
  DEFS += -DDEBUG
  COPTIONS += -g
  LOPTIONS += -g
else
  DEFS += -D_NDEBUG
  COPTIONS += -s -O2
  LOPTIONS += -s -O2
endif

ifneq ($(BASICLIBDIR),)
  COPTIONS += -I$(BASICLIBDIR)
endif

COPTIONS += -I$(GCDIR) $(WARNS) $(DEFS) -fPIC -fasm -finline-functions
COPTIONS += -fno-omit-frame-pointer -c -o
LOPTIONS += -Wall -fPIC -shared -Wl,-soname,$(DYNLIBMAJOR) -o

include sources

OBJS1 := $(SRCS:.c=.o)
OBJS := $(OBJS1:.cpp=.o)

# end of default.mak ...

