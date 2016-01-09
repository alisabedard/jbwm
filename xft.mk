# Titlebar Xft support:
DEFINES += -DUSE_XFT
EXTRALIBS += -lXft
EXTRAINCLUDES += `pkg-config --cflags xft`

# Not necessary except on old NetBSD, for Xft support:
EXTRAINCLUDES += -I/usr/pkg/include/freetype2 -I/usr/X11R6/include/freetype2
