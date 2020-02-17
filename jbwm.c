// jbwm - Minimalist Window Manager for X
// Copyright 2008-2019, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
//#undef DEBUG
#include "jbwm.h"
#include "config.h"
#include "ewmh.h"
#include "font.h"
#include "JBWMScreen.h"
#include "keys.h"
#include "log.h"
#include "new.h"
#include <stdbool.h>
#include "util.h"
#ifdef JBWM_USE_XFT
static void allocate_xft_color(Display * d, struct JBWMScreen * s)
{
    XftColorAllocName(d, DefaultVisualOfScreen(s->xlib),
        DefaultColormapOfScreen(s->xlib), JBWM_FG,
        &s->font_color);
}
#endif//JBWM_USE_XFT
static void allocate_colors(struct JBWMScreen * restrict s)
{
    Display * d=s->display;
    const uint8_t n = s->id;
#define PIX(field, color) s->pixels.field = jbwm_get_pixel(d, n, color);
    PIX(bg, JBWM_BG);
    PIX(fc, JBWM_FC);
    PIX(fg, JBWM_FG);
    PIX(close, JBWM_CLOSE);
    PIX(resize, JBWM_RESIZE);
    PIX(shade, JBWM_SHADE);
    PIX(stick, JBWM_STICK);
#undef PIX
#ifdef JBWM_USE_XFT
    allocate_xft_color(d, s);
#endif//JBWM_USE_XFT
}
static bool check_redirect(Display * d, const Window w)
{
    XWindowAttributes a;
    XGetWindowAttributes(d, w, &a);
    JBWM_LOG("check_redirect(0x%x): override_redirect: %s, "
        "map_state: %s", (int)w,
        a.override_redirect ? "true" : "false",
        a.map_state == IsViewable ? "IsViewable"
        : "not IsViewable");
    return (!a.override_redirect && (a.map_state == IsViewable));
}
// Free returned data with XFree()
static Window * get_windows(Display * dpy, const Window root,
    uint16_t * win_count)
{
    Window * w, d;
    unsigned int n;
    XQueryTree(dpy, root, &d, &d, &w, &n);
    *win_count = n;
    return w;
}
static void setup_clients(Display * d, struct JBWMScreen * s)
{
    uint16_t n;
    Window * w = get_windows(d, RootWindow(d, s->id), &n);
    JBWM_LOG("Started with %d clients", n);
    if (w) { // Avoid segmentation fault on empty list.
        while(n--)
            if(check_redirect(d,w[n]))
                jbwm_new_client(s,w[n]);
        XFree(w);
    }
}
static inline void setup_gc(Display * d, struct JBWMScreen * s)
{
    XGCValues v={
        .foreground = s->pixels.fg,
        .background = s->pixels.bg,
        .subwindow_mode = IncludeInferiors,
        .line_width = 1
    };
    unsigned long mask = GCSubwindowMode | GCLineWidth
    | GCForeground | GCBackground;
#ifndef JBWM_USE_XFT
    v.font=s->font->fid;
    mask|=GCFont; 
#endif//JBWM_USE_XFT
    s->gc=DefaultGC(d,s->id);
    XChangeGC(d,s->gc,mask,&v);
    s->border_gc=XCreateGC(d,s->xlib->root,GCFunction,
        &(XGCValues){.function=GXxor});
    XCopyGC(d,s->gc,mask,s->border_gc);
}
static inline void setup_event_listeners(Display * d, const Window root)
{
    enum {
        EMASK = SubstructureRedirectMask | SubstructureNotifyMask |
        EnterWindowMask | PropertyChangeMask
        | ColormapChangeMask
    };
    XChangeWindowAttributes(d, root, CWEventMask,
        &(XSetWindowAttributes){.event_mask = EMASK });
}
#ifdef JBWM_USE_XFT
/* Create a unique XftDraw for each screen to properly handle colormaps and
 * screen limitations.  */
static XftDraw * new_xft_draw(Screen * s)
{
    return XftDrawCreate(DisplayOfScreen(s), RootWindowOfScreen(s),
        DefaultVisualOfScreen(s), DefaultColormapOfScreen(s));
}
#endif//JBWM_USE_XFT
// Initialize SCREENS amount of screens.
void jbwm_init_screens(Display *d, struct JBWMScreen *s, const short screens)
{
    if(screens>=0){
        uint16_t n;
        unsigned long *lprop;
        JBWM_LOG("jbwm_init_screen(d, screens), screens is %d", screens);
        s->display=d;
        s->id = screens;
        s->xlib = ScreenOfDisplay(d, screens);
        lprop=jbwm_get_property(d,s->xlib->root,
            XInternAtom(d,"_NET_CURRENT_DESKTOP",false),&n);
        if(n){
            s->vdesk=lprop[0];
            XFree(lprop);
        }else
            s->vdesk=0;
#ifdef JBWM_USE_XFT
        s->xft = new_xft_draw(s->xlib);
#else//!JBWM_USE_XFT
        s->font = jbwm_get_font();
#endif//JBWM_USE_XFT
        s->font_height=jbwm_get_font_height();
        allocate_colors(s);
        setup_gc(d, s);
        { // r scope
            const Window r = RootWindow(d, screens);
            setup_event_listeners(d, r);
            jbwm_grab_root_keys(d, r);
        }
        /* scan all the windows on this screen */
        setup_clients(d, s);
        jbwm_ewmh_init_screen(d, s);
        jbwm_init_screens(d, s, screens - 1);
    }
}
