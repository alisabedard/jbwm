# If you don't use CC 
#CC       = gcc
#CC = clang
#CFLAGS=-O4

# Edit this line if you don't want arwm to install under /usr.
# Note that $(DESTDIR) is used by the Debian build process.
#prefix = $(DESTDIR)/usr
# For typical manually built package:
#prefix = /usr/local
# For custom stash script
prefix = /usr/local/stash/arwm
# For use with NetBSD linkfarm or GNU stow.
#prefix = /usr/local/packages/arwm
# For local home install, managed by GNU stow.
#prefix = ~/opt/stow/arwm

XROOT    = /usr/X11R6
INCLUDES += -I$(XROOT)/include
LDPATH  += -L$(XROOT)/lib

LIBS += -lX11 


DEFINES  = $(EXTRA_DEFINES) 

# Configure arwm by editing the following DEFINES lines.  You can also
# add options by setting EXTRA_DEFINES on the make(1) command line,
# e.g., make EXTRA_DEFINfES="-DDEBUG".


#CFLAGS += -Wall -W -pedantic -std=c89
# Optimization:
#CFLAGS += -Os -ffast-math -fomit-frame-pointer 
# Machine-specific:
#CFLAGS += -mfpmath=both -mpc64 -floop-parallelize-all
CFLAGS += -Os 
# Debugging:
#CFLAGS = -ggdb -pg
#DEFINES += -DDEBUG

#DEFINES += -D__USE_GNU

# Uncomment to enable SHAPE extension support
#DEFINES += -DUSE_SHAPE
#LIBS += -lXext # Required for SHAPE extension

# Titlebar xbm/xpm support:  
#DEFINES += -DUSE_XBM
# XPM support is beautiful but substantially increases binary size.
#DEFINES += -DUSE_XPM
#LIBS	+= -lXpm

# Titlebar gradient support:
#DEFINES += -DUSE_GRADIENT

# Titlebar Xft support:
# Warning, Xft impedes performance and leaks memory.
DEFINES += -DUSE_XFT
LIBS += -lXft
INCLUDES += `pkg-config --cflags xft`
#INCLUDES += -I/usr/pkg/include/freetype2 -I/usr/X11R6/include/freetype2


# Uncomment to enable colormap support
#DEFINES += -DUSE_CMAP

# Uncomment to enable display of window geometry properties on resize.
# Disabling this information should shorten the execution time of DRAW_OUTLINE,
# which is often at the top of the profiled function count.  

# DEFINES += -DINFORMATION_ON_OUTLINE

# Uncomment the following line if you want to use Ctrl+Alt+q to kill windows
# instead of Ctrl+Alt+Escape (or just set it to what you want).  This is
# useful under XFree86/Cygwin and VMware (probably)
#DEFINES += -DKEY_KILL=XK_q

# Uncomment to enable EWMH support.  Currently buggy and incomplete
#DEFINES += -DUSE_EWMH
#SRCS+= ewmh.c 

# Uncomment to enable parsing command line arguments.
#DEFINES += -DUSE_ARGV

# Uncomment to enable titlebars
DEFINES += -DUSE_TBAR
SRCS += titlebar.c

# Uncomment to enable window snapping.
DEFINES += -DUSE_SNAP

# Uncomment to enable shade button (buggy)
# DEFINES += -DUSE_SHADE

# ----- You shouldn't need to change anything under this line ------ #

version = 1.29

distname = arwm-$(version)

DEFINES += -DVERSION=\"$(version)\" $(DEBIAN)
CFLAGS  += $(INCLUDES) $(DEFINES) 
# Profiling flag.
#CFLAGS += -pg -O0
# Maximize warnings.
CFLAGS += -Wall -Wextra -Werror 
LDFLAGS += $(LDPATH) $(LIBS)
# Uncomment for static linking of binary:
#LDFLAGS += -static 

HEADERS  = arwm.h log.h
SRCS += client.c events.c arwm.c misc.c new.c screen.c ARWMButton.c
SRCS += graphics.c key_event.c configure_event.c button_event.c keymap.c
OBJS     = $(SRCS:.c=.o)

.PHONY: all install dist debuild clean

all: arwm

# Uncomment this target and comment out the other arwm target
# if you want to save ~400 more bytes

#arwm: $(SRCS) $(HEADERS)
#	rm -f all.c
#	cat *.h *.c > all.c
#	$(CC) $(CFLAGS) all.c -o $@ $(LDFLAGS)
#	rm -f all.c


arwm: $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

INSTALL=install -c
INSTALL_PROG=$(INSTALL) -s
INSTALL_DIR=install -d

install: arwm
	if [ -f arwm.exe ]; then mv arwm.exe arwm; fi
	$(INSTALL_DIR) $(prefix)/bin $(prefix)/share/man/man1
	$(INSTALL_PROG) arwm $(prefix)/bin
	$(INSTALL) arwm.1 $(prefix)/share/man/man1

dist:
	darcs dist --dist-name $(distname)
	mv $(distname).tar.gz ..

debuild: dist
	-cd ..; rm -rf $(distname)/ $(distname).orig/
	cd ..; mv $(distname).tar.gz arwm_$(version).orig.tar.gz
	cd ..; tar xfz arwm_$(version).orig.tar.gz
	cp -a debian ../$(distname)/
	rm -rf ../$(distname)/debian/_darcs/
	cd ../$(distname); debuild

clean:
	rm -f arwm arwm.exe $(OBJS) *.core

distclean: clean
	rm -f *~ *.out .*.swp .*.swn

archive:
	cd ..;  tar cjf arwm-$(version).tar.bz2 arwm

# DO NOT DELETE
