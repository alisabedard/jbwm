# If you don't use CC 
CC       = gcc

# Edit this line if you don't want arwm to install under /usr.
# Note that $(DESTDIR) is used by the Debian build process.
#prefix = $(DESTDIR)/usr
# For typical manually built package:
prefix = /usr/local
# For use with NetBSD linkfarm or GNU stow.
#prefix = /usr/local/packages/arwm
# For local home install, managed by GNU stow.
#prefix = ~/opt/stow/arwm

XROOT    = /usr/X11R6
INCLUDES += -I/usr/pkg/include/freetype2
INCLUDES += -I/usr/X11R6/include/freetype2
INCLUDES += -I/usr/include/freetype2
INCLUDES += -I/usr/pkg/include
INCLUDES += -I$(XROOT)/include
LDPATH  += -L/usr/pkg/lib
LDPATH  += -L$(XROOT)/lib

LIBS    += -lX11 -lXdmcp -lXau

DEFINES  = $(EXTRA_DEFINES)

# Configure arwm by editing the following DEFINES lines.  You can also
# add options by setting EXTRA_DEFINES on the make(1) command line,
# e.g., make EXTRA_DEFINES="-DDEBUG".

# Print whatever debugging messages I've left in this release.
#DEFINES += -DDEBUG	# miscellaneous debugging
#DEFINES += -DTITLEBAR_DEBUG # debug titlebar.  
#DEFINES += -DGRAPHICS_DEBUG # debug graphics.  
#DEFINES += -DXDEBUG	# show some X calls
#CFLAGS+=-g3

# Titlebar xbm/xpm support:  
#DEFINES += -DUSE_XBM
#DEFINES += -DUSE_XPM
#LIBS	+= -lXpm

# Titlebar Xft support:
DEFINES += -DUSE_XFT
LIBS += -lXft
LIBS += -lXext
INCLUDES += -I/usr/pkg/include/freetype2 -I/usr/X11R6/include/freetype2

# Uncomment to enable borders around windows when resizing.  Disable on
# systems with limited CPU cycles to spare.
DEFINES += -DWITHRESIZEBORDER

# Uncomment to enable display of window geometry properties on resize.
# WITHRESIZEBORDER must be enabled, otherwise this option has no effect.
# Disabling this information should shorten the execution time of DRAW_OUTLINE,
# which is often at the top of the profiled function count.  
#DEFINES += -DINFORMATION_ON_OUTLINE

# Uncomment to enable solid window drags.  This can be slow on old systems.
#DEFINES += -DSOLIDDRAG

# Uncomment to enable a titlebar using the infobanner.  
# INFOBANNER must also be enabled.
DEFINES += -DTITLEBAR

# Uncomment to use a filled rectangle when dragging.  
#DEFINES += -DFILLED_DRAG

# To support shaped windows properly, uncomment the following two lines:
#DEFINES += -DSHAPE
#LIBS	+= -lXext

# Uncomment to enable changing a window's desktop by scrolling on the
# title bar.  
# DEFINES += -DSCROLL_WHEEL_PAGER

# Uncomment for snap-to-border support (thanks, Neil Drumm)
# Start arwm with -snap num to configure (num is proximity in pixels to snap to)
# Default value is 4.  
DEFINES += -DSNAP

# You can save a few bytes if you know you won't need colour map support
# (e.g., for 16 or more bit displays)
#DEFINES += -DCOLOURMAP

# Uncomment the following line if you want to use Ctrl+Alt+q to kill windows
# instead of Ctrl+Alt+Escape (or just set it to what you want).  This is
# useful under XFree86/Cygwin and VMware (probably)
#DEFINES += -DKEY_KILL=XK_q


# ----- You shouldn't need to change anything under this line ------ #

version = 1.23

distname = arwm-$(version)

DEFINES += -DVERSION=\"$(version)\" $(DEBIAN)
CFLAGS  += $(INCLUDES) $(DEFINES) -Os -Wall
# Profiling flag.
#CFLAGS += -pg -O0
# Debugging flag.
#CFLAGS  += $(INCLUDES) $(DEFINES) -g -Wall
# Maximize warnings.
CFLAGS += -Wall -W 
LDFLAGS += $(LDPATH) $(LIBS)
# Uncomment for static linking of binary:
#LDFLAGS += -static 

CFLAGS += -Os -ffast-math

HEADERS  = arwm.h log.h
SRCS= client.c events.c arwm.c misc.c new.c screen.c ewmh.c titlebar.c
SRCS+= graphics.c key_event.c configure_event.c button_event.c
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
	rm -f arwm arwm.exe $(OBJS) *.core *~
# DO NOT DELETE
