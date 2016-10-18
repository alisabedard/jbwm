# jbwm - Minimalist Window Manager for X
# Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
# Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
# See README for license and other details.
version=1.49
PROG=jbwm
distname=$(PROG)-$(version)
# Edit/override this line if you don't want jbwm to install under /usr.
PREFIX=/usr
# Note that $(DESTDIR) is used by the Debian build process.
dest=$(DESTDIR)$(PREFIX)
# FreeBSD:
CFLAGS+=-I/usr/local/include
LDFLAGS+=-L/usr/local/lib
# OpenBSD:
CFLAGS+=-I/usr/X11R6/include
CFLAGS+=-I/usr/X11R6/include/freetype2
LDFLAGS+=-L/usr/X11R6/lib
# NetBSD:
CFLAGS+=-I/usr/X11R7/include
CFLAGS+=-I/usr/X11R7/include/freetype2
CFLAGS+=-Wno-missing-field-initializers
#LDFLAGS+=-Wl,-R/usr/X11R6/lib
LDFLAGS+=-L/usr/X11R7/lib
LDFLAGS+=-Wl,-R/usr/X11R7/lib
LDFLAGS+=-lX11 
# Uncomment to enable X11 miscellaneous debugging (events)
#CFLAGS+=-DXDEBUG
CFLAGS+=-DVERSION=\"$(version)\" $(DEBIAN)
CFLAGS+=-D_XOPEN_SOURCE=700 -std=c11
# Uncomment for static linking of binary:
#LDFLAGS+=-static 
OBJS+=client.o events.o jbwm.o new.o screen.o 
OBJS+=button_event.o keys.o util.o max.o
$(PROG): $(OBJS) 
	$(CC) $(LDFLAGS) $(OBJS) -o $@
	strip $(PROG) -o $(PROG).tmp
	ls -l $(PROG).tmp >> sz.log
	rm -f $(PROG).tmp
	tail sz.log
all: $(PROG)
strip: 
	# Leave .plt.got
	strip -s \
		-R .jcr \
		-R .eh_frame \
		-R .got \
		-R .note \
		-R .gnu.version \
		-R .note.gnu.build-id \
		-R .note.ABI-tag \
		-R .eh_frame_hdr \
		-R .comment \
		-R .gnu.hash \
		-R .shstrtab \
		$(PROG)
INSTALL=install -c
INSTALL_PROG=$(INSTALL) 
INSTALL_DIR=install -d
install: 
	$(INSTALL_DIR) $(dest)/bin $(dest)/share/man/man1
	$(INSTALL_PROG) $(PROG) $(dest)/bin
	$(INSTALL) $(PROG).1 $(dest)/share/man/man1
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
	make -f Makefile.debug -j 4
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
