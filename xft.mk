# Titlebar Xft support:
jbwm_cflags+=-DJBWM_USE_XFT
jbwm_cflags+=`pkg-config --cflags xft`
jbwm_ldflags+=-lXft
# NetBSD:
jbwm_cflags+=-I/usr/pkg/include/freetype2
jbwm_cflags+=-I/usr/X11R6/include/freetype2
