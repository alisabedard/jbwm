# Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
# Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
# See README for license and other details.
# Debugging symbols, enabled to be able to debug core dumps
#CFLAGS+=-ggdb
# Link time optimization greatly reduces binary size:
# However, it may not work correctly with clang.
#CFLAGS+=-flto -O3
#CFLAGS+=-ggdb
# Enable all warnings
CFLAGS+=-Wall -Wextra
# Uncomment to enable SHAPE extension support
include shape.mk
# Titlebar Xft support:
# Warning: valgrind reports that xft leaks memory.
#include xft.mk
# Uncomment to enable parsing command line arguments.
#  Saves ~2030 bytes
CFLAGS+=-DUSE_ARGV
# Uncomment to enable title bars
include title_bar.mk
# Uncomment to enable window snapping. ~4k
include snap.mk
# Uncomment to enable STDIO
CFLAGS+=-DSTDIO
# Uncomment to enable EWMH ~8k
include ewmh.mk
# Uncomment to enable MWM hints
include mwm.mk
include jbwm.mk
