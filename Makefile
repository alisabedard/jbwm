#CC = clang
#CC=/opt/diet/bin/diet -Os gcc
# Compile for minimum size
#CFLAGS=-Os
# Compile for current machine
#CFLAGS+=-march=native
# Tuning
#CFLAGS+=-fomit-frame-pointer -pipe -flto
# No optimizatio
CFLAGS=-O0
# Debug
#CFLAGS=-O0 -ggdb 
#CFLAGS+=-DDEBUG -DSTDIO
# Profile
#CFLAGS+=-pg
# Enable all warnings
CFLAGS+=-W -Wall -Wextra

# Edit/override this line if you don't want jbwm to install under /usr.
PREFIX=/usr
# Note that $(DESTDIR) is used by the Debian build process.
prefix = $(DESTDIR)/$(PREFIX)

# Fix build on NetBSD and older systems:
INCLUDES+=-I/usr/X11R6/include
INCLUDES+=-I/usr/X11R7/include
LDPATH+=-L/usr/X11R6/lib
LDPATH+=-L/usr/X11R7/lib

LIBS += -lX11 

DEFINES  = $(EXTRA_DEFINES) 

# Configure jbwm by editing the following DEFINES lines.  You can also
# add options by setting EXTRA_DEFINES on the make(1) command line,

# Comment out to enable assertions
#DEFINES += -DNDEBUG

# Uncomment to enable SHAPE extension support
DEFINES += -DUSE_SHAPE
EXTRALIBS += -lXext # Required for SHAPE extension

# Titlebar Xft support:
# Warning, Xft impedes performance and leaks memory.
#DEFINES += -DUSE_XFT
#EXTRALIBS += -lXft
#EXTRAINCLUDES += `pkg-config --cflags xft`

# Not necessary except on old NetBSD, for Xft support:
#EXTRAINCLUDES += -I/usr/pkg/include/freetype2 -I/usr/X11R6/include/freetype2

# Uncomment to enable colormap support
# Saves 600 bytes
#DEFINES+= -DUSE_CMAP

# Uncomment to enable parsing command line arguments.
#  Saves ~2030 bytes
#DEFINES += -DUSE_ARGV 

# Uncomment to enable titlebars
DEFINES += -DUSE_TBAR
EXTRASRCS += titlebar.c 

# Uncomment to enable window snapping. (Adds 4k bytes)
DEFINES += -DUSE_SNAP

# Uncomment to enable STDIO
DEFINES += -DSTDIO

# Uncomment to enable EWMH (Adds ~4k bytes)
DEFINES += -DEWMH
EXTRASRCS += ewmh.c

# Uncomment to enable MWM hints
DEFINES += -DMWM

# Uncomment to disable everything for minimal configuration 
#CFLAGS=-Os
#DEFINES=-DNDEBUG
#EXTRASRCS=
#EXTRALIBS=
#EXTRAINCLUDES=

# ----- You shouldn't need to change anything under this line ------ #

version = 1.39

PROG=jbwm
distname = $(PROG)-$(version)

SRCS+=$(EXTRASRCS)
INCLUDES+=$(EXTRAINCLUDES)
LIBS+=$(EXTRALIBS)

# Uncomment to enable X11 miscellaneous debugging (events)
#DEFINES += -DXDEBUG

DEFINES += -DVERSION=\"$(version)\" $(DEBIAN)
CFLAGS  += $(INCLUDES) $(DEFINES) 
CFLAGS += -D_XOPEN_SOURCE=700 -std=c99
LDFLAGS += $(LDPATH) $(LIBS)
# Uncomment for static linking of binary:
#LDFLAGS += -static 

SRCS += client.c events.c jbwm.c new.c screen.c 
SRCS += graphics.c button_event.c keys.c
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

# DO NOT DELETE
