jbwm_cflags+=-DJBWM_USE_TITLE_BAR
objects+=title_bar.o font.o
# Titlebar Xft support:
jbwm_cflags+=`pkg-config --cflags xft`
jbwm_ldflags+=`pkg-config --libs xft`
