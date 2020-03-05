// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_CLIENT_STRUCT_H
#define JBWM_CLIENT_STRUCT_H
#include <X11/Xlib.h>
#include "JBWMClientOptions.h"
#include "JBWMClientTitleBar.h"
#include "JBWMRectangle.h"
#include "JBWMScreen.h"
struct JBWMScreen;
struct JBWMClient {
//    struct JBWMClient ** head; // store to avoid call overhead
//    struct JBWMClient ** current_client;
    struct JBWMClient * next;
    struct JBWMScreen * screen;
    struct JBWMClientTitleBar tb;
    Colormap cmap;
    Window window, parent;
    struct JBWMClientOptions opt;
    union JBWMRectangle size, old_size, before_fullscreen;
    uint8_t vdesk;
    int8_t ignore_unmap;
};
#endif /* JBWM_CLIENT_STRUCT_H */
