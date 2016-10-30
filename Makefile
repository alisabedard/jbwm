# Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
# Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
# See README for license and other details.
# Debugging symbols, enabled to be able to debug core dumps
#jbwm_cflags+=-ggdb
# Link time optimization greatly reduces binary size:
# However, it may not work correctly with clang.
#jbwm_cflags+=-flto -O3
#jbwm_cflags+=-ggdb
# Enable all warnings
jbwm_cflags+=-Wall -Wextra
# Uncomment to enable SHAPE extension support
include shape.mk
# Titlebar Xft support:
# Warning: valgrind reports that xft leaks memory.
#include xft.mk
# Uncomment to enable title bars
include title_bar.mk
# Uncomment to enable EWMH ~8k
include ewmh.mk
include jbwm.mk
