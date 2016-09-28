# Titlebar Xft support:
CFLAGS+=-DUSE_XFT
CFLAGS+=`pkg-config --cflags xft`
LDFLAGS+=-lXft
# NetBSD:
CFLAGS+=-I/usr/pkg/include/freetype2
CFLAGS+=-I/usr/X11R6/include/freetype2
