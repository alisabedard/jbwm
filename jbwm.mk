# jbwm - Minimalist Window Manager for X
# Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
# Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
# See README for license and other details.

version = 1.48

PROG=jbwm
distname = $(PROG)-$(version)

# Edit/override this line if you don't want jbwm to install under /usr.
PREFIX=/usr
# Note that $(DESTDIR) is used by the Debian build process.
prefix = $(DESTDIR)$(PREFIX)

SRCS+=$(EXTRASRCS)
INCLUDES+=$(EXTRAINCLUDES)
LIBS+=$(EXTRALIBS)

# Fix build on FreeBSD
INCLUDES+=-I/usr/local/include
LDPATH+=-L/usr/local/lib

# Fix build on NetBSD and older systems:
# Uncomment X11R6 paths for older installations.
#INCLUDES+=-I/usr/X11R6/include
INCLUDES+=-I/usr/X11R7/include

# NetBSD now requires RPATH set
#
#LDPATH+=-L/usr/X11R6/lib -Wl,-R/usr/X11R6/lib
LDPATH+=-L/usr/X11R7/lib -Wl,-R/usr/X11R7/lib
LIBS += -lX11 

# Uncomment to enable X11 miscellaneous debugging (events)
#DEFINES += -DXDEBUG

DEFINES += -DVERSION=\"$(version)\" $(DEBIAN)
CFLAGS  += $(INCLUDES) $(DEFINES) 
CFLAGS += -D_XOPEN_SOURCE=700 -std=c11
LDFLAGS += $(LDPATH) $(LIBS)
# Uncomment for static linking of binary:
#LDFLAGS += -static 

SRCS += client.c events.c jbwm.c new.c screen.c 
SRCS += button_event.c keys.c util.c max.c
OBJS = $(SRCS:.c=.o)

$(PROG): $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)
	strip $(PROG) -o $(PROG).tmp
	ls -l $(PROG).tmp >> sz.log
	rm -f $(PROG).tmp
	tail sz.log

all: $(PROG)

strip: 
	strip -s \
		-R .comment \
		-R .note \
		-R .gnu.version \
		-R .note.gnu.build-id \
		-R .note.ABI-tag \
		-R .gnu.hash \
		-R .jcr \
		-R .plt.got \
		-R .eh_frame \
		-R .eh_frame_hdr \
		-R .got \
		$(PROG)

INSTALL=install -c
INSTALL_PROG=$(INSTALL) 
INSTALL_DIR=install -d
install: 
	$(INSTALL_DIR) $(prefix)/bin $(prefix)/share/man/man1
	$(INSTALL_PROG) $(PROG) $(prefix)/bin
	$(INSTALL) $(PROG).1 $(prefix)/share/man/man1

clean:
	rm -f $(PROG) *.o

distclean: clean
	rm -f *~ *.out .*.swp .*.swn *.orig .*~ *~~

archive: distclean
	cd ..;  tar cJf $(PROG)-$(version).tar.xz $(PROG)
 
tags:
	ctags *

indent:
	indent *.[ch]
	sed 's/ \/\//\/\//g' -i'~~' *.[ch]

debug:
	make clean
	make -f Makefile.debug

small:
	make clean
	make -f Makefile.small
	cp jbwm jbwm.small.dbg
	make strip
	ls -l jbwm >> small.log
	tail small.log

clang:
	make clean
	make -f Makefile.clang

tbonly:
	make clean
	make -f Makefile.tbonly

check:
	CFLAGS='-Werror'
	make clean
	make -f Makefile.clang
	make clean
	make -f Makefile.debug
	make clean
	make -f Makefile.small
	make clean
	make -f Makefile.tbonly
	make clean
	make # Default config
