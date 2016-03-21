# Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
# Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
# See README for license and other details.

#CFLAGS=-Ofast
#CFLAGS+=-Os
# Compile for current machine
#CFLAGS+=-march=native
# Link time optimization greatly reduces binary size:
# However, it may not work correctly with clang.
CFLAGS+=-flto
# Tuning
#CFLAGS+=-fomit-frame-pointer
# Enable all warnings
CFLAGS+=-W -Wall -Wextra

# Uncomment to disable assertions
#DEFINES += -DNDEBUG

# Uncomment to enable SHAPE extension support
DEFINES += -DUSE_SHAPE
EXTRALIBS += -lXext # Required for SHAPE extension

# Titlebar Xft support:
#include xft.mk

# Uncomment to enable parsing command line arguments.
#  Saves ~2030 bytes
DEFINES += -DUSE_ARGV 

# Uncomment to enable titlebars
DEFINES += -DUSE_TBAR
EXTRASRCS += titlebar.c 

# Uncomment to enable window snapping. ~4k
include snap.mk

# Uncomment to enable STDIO
DEFINES += -DSTDIO

# Uncomment to enable EWMH ~8k
DEFINES += -DEWMH
EXTRASRCS += ewmh.c

# Uncomment to enable MWM hints
include mwm.mk

# Uncomment to enable window gravity code
#DEFINES += -DGRAVITY

# Uncomment to disable window outlines, and use solid drag/resize.
# Saves ~300 bytes
# Caveat: Slow
#DEFINES += -DSOLID

include jbwm.mk
