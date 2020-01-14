jbwm_cflags+=-DJBWM_USE_XFT
jbwm_cflags+=`pkg-config --cflags xft`
jbwm_ldflags+=`pkg-config --libs xft`
