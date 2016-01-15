# jbwm - Minimalist Window Manager for X
# Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
# Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
# See README for license and other details.

#CC = clang
CFLAGS=-Os
# Compile for current machine
CFLAGS+=-march=native
# Tuning
CFLAGS+=-fomit-frame-pointer -pipe -flto
# Enable all warnings
CFLAGS+=-W -Wall -Wextra

# Uncomment to disable assertions
DEFINES += -DNDEBUG

# Uncomment to enable SHAPE extension support
DEFINES += -DUSE_SHAPE
EXTRALIBS += -lXext # Required for SHAPE extension

# Titlebar Xft support:
#include xft.mk

# Uncomment to enable colormap support
# Saves 600 int8_ts
#DEFINES+= -DUSE_CMAP

# Uncomment to enable parsing command line arguments.
#  Saves ~2030 int8_ts
DEFINES += -DUSE_ARGV 

# Uncomment to enable titlebars
DEFINES += -DUSE_TBAR
EXTRASRCS += titlebar.c 

# Uncomment to enable window snapping. (Adds 4k int8_ts)
include snap.mk

# Uncomment to enable STDIO
DEFINES += -DSTDIO

# Uncomment to enable EWMH (Adds ~4k int8_ts)
DEFINES += -DEWMH
EXTRASRCS += ewmh.c

# Uncomment to enable MWM hints
DEFINES += -DMWM

include jbwm.mk
