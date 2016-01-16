# jbwm - Minimalist Window Manager for X
# Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
# Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
# See README for license and other details.

version = 1.41

PROG=jbwm
distname = $(PROG)-$(version)

# Edit/override this line if you don't want jbwm to install under /usr.
PREFIX=/usr
# Note that $(DESTDIR) is used by the Debian build process.
prefix = $(DESTDIR)/$(PREFIX)

SRCS+=$(EXTRASRCS)
INCLUDES+=$(EXTRAINCLUDES)
LIBS+=$(EXTRALIBS)

# Fix build on NetBSD and older systems:
INCLUDES+=-I/usr/X11R6/include
INCLUDES+=-I/usr/X11R7/include
LDPATH+=-L/usr/X11R6/lib
LDPATH+=-L/usr/X11R7/lib
LIBS += -lX11 

# Uncomment to enable X11 miscellaneous debugging (events)
#DEFINES += -DXDEBUG

DEFINES += -DVERSION=\"$(version)\" $(DEBIAN)
CFLAGS  += $(INCLUDES) $(DEFINES) 
CFLAGS += -D_XOPEN_SOURCE=700 -std=c99
LDFLAGS += $(LDPATH) $(LIBS)
# Uncomment for static linking of binary:
#LDFLAGS += -static 

SRCS += client.c events.c jbwm.c new.c screen.c 
SRCS += button_event.c keys.c 
OBJS = $(SRCS:.c=.o)

$(PROG): $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

all: $(PROG)

strip: $(PROG)
	strip --strip-all --remove-section=.comment \
		--remove-section=.note $(PROG)

INSTALL=install -c
INSTALL_PROG=$(INSTALL) -s
INSTALL_DIR=install -d
install: strip
	$(INSTALL_DIR) $(prefix)/bin $(prefix)/share/man/man1
	$(INSTALL_PROG) $(PROG) $(prefix)/bin
	$(INSTALL) $(PROG).1 $(prefix)/share/man/man1

clean:
	rm -f $(PROG) *.o

distclean: clean
	rm -f *~ *.out .*.swp .*.swn

archive: distclean
	cd ..;  tar cJf $(PROG)-$(version).tar.xz $(PROG)

