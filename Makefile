#CC = clang
#CC=/opt/diet/bin/diet -Os gcc
CFLAGS=-Os -W -Wall
# Debug:
CFLAGS+=-ggdb -Werror
CFLAGS=-O0 -ggdb -DDEBUG -DSTDIO -W -Wall -Werror

# Profile:
#CFLAGS += -pg 

# Edit this line if you don't want jbwm to install under /usr.
# Note that $(DESTDIR) is used by the Debian build process.
prefix = $(DESTDIR)/usr
# For typical manually built package:
#prefix = /usr/local

# Fix build on NetBSD and older systems:
INCLUDES+=-I/usr/X11R6/include
INCLUDES+=-I/usr/X11R7/include
LDPATH+=-L/usr/X11R6/lib
LDPATH+=-L/usr/X11R7/lib

LIBS += -lX11 

DEFINES  = $(EXTRA_DEFINES) 

# Configure jbwm by editing the following DEFINES lines.  You can also
# add options by setting EXTRA_DEFINES on the make(1) command line,
# e.g., make EXTRA_DEFINES="-DDEBUG".

# Uncomment to debug and profile
#CFLAGS = -O0 -ggdb -pg
#DEFINES += -DDEBUG

# Uncomment to enable SHAPE extension support
#DEFINES += -DUSE_SHAPE
#LIBS += -lXext # Required for SHAPE extension

# XPM support is beautiful but substantially increases binary size.
#DEFINES += -DUSE_XPM
#LIBS	+= -lXpm

# Titlebar Xft support:
# Warning, Xft impedes performance and leaks memory.
#DEFINES += -DUSE_XFT
#LIBS += -lXft
#INCLUDES += `pkg-config --cflags xft`

# Not necessary except on old NetBSD, for Xft support:
#INCLUDES += -I/usr/pkg/include/freetype2 -I/usr/X11R6/include/freetype2

# Uncomment to enable colormap support
# Saves 600 bytes
#DEFINES += -DUSE_CMAP

# Uncomment to enable parsing command line arguments.
#  Saves ~2030 bytes
#DEFINES += -DUSE_ARGV

# Uncomment to enable titlebars
DEFINES += -DUSE_TBAR
SRCS += titlebar.c 

# Uncomment to use a colored titlebar theme (no size difference):
# DEFINES += -DUSE_COLORS

# Uncomment to enable window snapping.
# DEFINES += -DUSE_SNAP

# Uncomment to enable STDIO
#DEFINES += -DSTDIO

# Uncomment to enable EWMH
DEFINES += -DEWMH
SRCS += ewmh.c

# ----- You shouldn't need to change anything under this line ------ #

version = 1.37

PROG=jbwm
distname = $(PROG)-$(version)

DEFINES += -DVERSION=\"$(version)\" $(DEBIAN)
CFLAGS  += $(INCLUDES) $(DEFINES) 
CFLAGS += -D_XOPEN_SOURCE=700
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

archive:
	cd ..;  tar cJf $(PROG)-$(version).tar.xz $(PROG)

# DO NOT DELETE
