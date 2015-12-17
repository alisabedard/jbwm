#CC = clang

CFLAGS=-Os -W -Wall

# Debug:
#CFLAGS=-O0 -ggdb -DDEBUG -W -Wall -Werror

# Profile:
#CFLAGS += -pg 

# Edit this line if you don't want jbwm to install under /usr.
# Note that $(DESTDIR) is used by the Debian build process.
prefix = $(DESTDIR)/usr
# For typical manually built package:
#prefix = /usr/local

# For older *nix installs
XROOT    = /usr/X11R6
INCLUDES += -I$(XROOT)/include
LDPATH  += -L$(XROOT)/lib

# Fix build on NetBSD
XROOT    = /usr/X11R7
INCLUDES += -I$(XROOT)/include
LDPATH  += -L$(XROOT)/lib

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

# ----- You shouldn't need to change anything under this line ------ #

version = 1.37

distname = jbwm-$(version)

DEFINES += -DVERSION=\"$(version)\" $(DEBIAN)
CFLAGS  += $(INCLUDES) $(DEFINES) 
CFLAGS += -D_XOPEN_SOURCE=700
LDFLAGS += $(LDPATH) $(LIBS)
# Uncomment for static linking of binary:
#LDFLAGS += -static 

SRCS += client.c events.c jbwm.c misc.c new.c screen.c 
SRCS += graphics.c key_event.c button_event.c keymap.c
OBJS = $(SRCS:.c=.o)

jbwm: $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

all: jbwm

strip: jbwm
	strip --strip-all --remove-section=.comment \
		--remove-section=.note jbwm

INSTALL=install -c
INSTALL_PROG=$(INSTALL) -s
INSTALL_DIR=install -d
install: strip
	if [ -f jbwm.exe ]; then mv jbwm.exe jbwm; fi
	$(INSTALL_DIR) $(prefix)/bin $(prefix)/share/man/man1
	$(INSTALL_PROG) jbwm $(prefix)/bin
	$(INSTALL) jbwm.1 $(prefix)/share/man/man1

dist:
	darcs dist --dist-name $(distname)
	mv $(distname).tar.gz ..

debuild: dist
	-cd ..; rm -rf $(distname)/ $(distname).orig/
	cd ..; mv $(distname).tar.gz jbwm_$(version).orig.tar.gz
	cd ..; tar xfz jbwm_$(version).orig.tar.gz
	cp -a debian ../$(distname)/
	rm -rf ../$(distname)/debian/_darcs/
	cd ../$(distname); debuild

clean:
	rm -f jbwm jbwm.exe $(OBJS) *.core

distclean: clean
	rm -f *~ *.out .*.swp .*.swn

archive:
	cd ..;  tar cjf jbwm-$(version).tar.bz2 jbwm

# DO NOT DELETE
