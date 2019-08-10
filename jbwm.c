// jbwm - Minimalist Window Manager for X
// Copyright 2008-2019, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
//#undef DEBUG
#include "jbwm.h"
#include "JBWMScreen.h"
#include "config.h"
#include "ewmh.h"
#include "keys.h"
#include "log.h"
#include "new.h"
#include "screen.h"
#include "util.h"
// Macros:
#define ENV(e) JBWM_ENV_##e
static void allocate_xft_color(Display * d, struct JBWMScreen * s)
{
    XftColorAllocName(d, DefaultVisualOfScreen(s->xlib),
        DefaultColormapOfScreen(s->xlib), getenv(ENV(FG)),
        &s->font_color);
}
static void allocate_colors(Display * d, struct JBWMScreen * restrict s)
{
    const uint8_t n = s->id;
#define PIX(field, env) s->pixels.field = jbwm_get_pixel(d, n, getenv(env));
    PIX(bg, ENV(BG));
    PIX(fc, ENV(FC));
    PIX(fg, ENV(FG));
    PIX(close, ENV(CLOSE));
    PIX(resize, ENV(RESIZE));
    PIX(shade, ENV(SHADE));
    PIX(stick, ENV(STICK));
    allocate_xft_color(d, s);
#undef PIX
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
    return (Window *)w;
}
static void set_up_next_window(const int n, Display * d,
    struct JBWMScreen * restrict s, const Window * restrict w)
{
    if (n >= 0) {
        if (check_redirect(d, w[n]))
            jbwm_new_client(d, s, w[n]);
        set_up_next_window(n - 1, d, s, w);
    }
}
static void setup_clients(Display * d, struct JBWMScreen * s)
{
    uint16_t n;
    Window * w = get_windows(d, RootWindow(d, s->id), &n);
    JBWM_LOG("Started with %d clients", n);
    if (w) { // Avoid segmentation fault on empty list.
        set_up_next_window(n, d, s, w);
        XFree(w);
    }
}
static inline void setup_gc(Display * d, struct JBWMScreen * s)
{
    XChangeGC(d, DefaultGC(d, s->id), GCFunction | GCSubwindowMode |
        GCLineWidth | GCForeground | GCBackground,
        &(XGCValues){.foreground = s->pixels.fg, .background =
            s->pixels.bg, .function = GXxor, .subwindow_mode =
                IncludeInferiors, .line_width = 1});
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
/* Create a unique XftDraw for each screen to properly handle colormaps and
 * screen limitations.  */
static XftDraw * new_xft_draw(Screen * s)
{
    return XftDrawCreate(DisplayOfScreen(s), RootWindowOfScreen(s),
        DefaultVisualOfScreen(s), DefaultColormapOfScreen(s));
}
// Initialize SCREENS amount of screens.
void jbwm_init_screens(Display * d, const short screens)
{
    if (screens < 0)
        return;
    JBWM_LOG("jbwm_init_screen(d, screens), screens is %d", screens);
    struct JBWMScreen * restrict s = &jbwm_get_screens()[screens];
    s->id = screens;
    s->vdesk = 0;
    s->xlib = ScreenOfDisplay(d, screens);
    s->xft = new_xft_draw(s->xlib);
    allocate_colors(d, s);
    setup_gc(d, s);
    { // r scope
        const Window r = RootWindow(d, screens);
        setup_event_listeners(d, r);
        jbwm_grab_root_keys(d, r);
    }
    /* scan all the windows on this screen */
    setup_clients(d, s);
    jbwm_ewmh_init_screen(d, s);
    jbwm_init_screens(d, screens - 1);
}
void jbwm_set_defaults(void)
{
#define SETENV(i) setenv(ENV(i), JBWM_DEF_##i, 0)
    SETENV(FG); SETENV(FC); SETENV(BG); SETENV(TERM);
    SETENV(CLOSE); SETENV(RESIZE); SETENV(SHADE);
    SETENV(STICK); SETENV(FONT);
#undef SETENV
}
