/*  Copyright 2020, Jeffrey E. Bedard <jefbed@gmail.com> */
#include "drag.h"
#include "font.h"
#include "JBWMClient.h"
#include "move_resize.h"
#include "snap.h"
#include <stdlib.h>
#include <X11/cursorfont.h>
enum {
    JBWMMouseMask = ButtonPressMask | ButtonReleaseMask
    | PointerMotionMask
};
extern inline void jbwm_warp(Display * dpy, const Window w, const short x,
    const short y);
static Cursor get_font_cursor(Display * d)
{
    static Cursor c;
    if (!c)
        c = XCreateFontCursor(d, XC_fleur);
    return c;
}
static void grab_pointer(Display * d, const Window w)
{
    XGrabPointer(d, w, false, JBWMMouseMask, GrabModeAsync,
        GrabModeAsync, None, get_font_cursor(d), CurrentTime);
}
static void set_size(struct JBWMClient * restrict c,
    const int16_t * restrict p)
{
    union JBWMRectangle * restrict g = &c->size;
    g->width = abs(g->x - p[0]);
    g->height = abs(g->y - p[1]);
}
__attribute__((nonnull))
static void set_position(struct JBWMClient * restrict c,
    const int16_t * restrict original,
    const int16_t * restrict start,
    const int16_t * restrict p)
{
    c->size.x = original[0]-start[0]+p[0];
    c->size.y = original[1]-start[1]+p[1];
    jbwm_snap_client(c);
}
__attribute__((nonnull))
static void query_pointer(Display * dpy, Window w,
    int16_t * restrict p)
{
    int x, y;
    { /*  d, u scope */
        int d; /*  dummy integer */
        unsigned int u; /*  dummy unsigned integer */
        XQueryPointer(dpy, w, &w, &w, &d, &d, &x, &y, &u);
    }
    p[0] = x;
    p[1] = y;
}
__attribute__((nonnull))
static void draw_outline(struct JBWMClient * restrict c)
{
    const uint8_t fh=c->screen->font_height;
    const uint8_t o =  (c->opt.no_title_bar^1)*fh;
    const union JBWMRectangle * restrict g = &c->size;
    enum { BORDER = 1 };
    Display * d = c->screen->display;
    XDrawRectangle(d, c->screen->xlib->root, c->screen->border_gc,
        g->x, g->y - o, g->width + BORDER, g->height + BORDER + o);
}
static void drag_event_loop(struct JBWMClient * restrict c, const bool resize)
{
    const Window root = c->screen->xlib->root;
    const int16_t original[] = {c->size.x, c->size.y};
    int16_t start[2];
    Display * d = c->screen->display;
    query_pointer(d, root, start);
    const uint8_t b = c->opt.border;
    for (;;) {
        XEvent e;
        int16_t p[2];
        XMaskEvent(d, JBWMMouseMask, &e);
        /*  Quit drag loop if any other event. */
        if(e.type != MotionNotify)
            break;
        p[0]=e.xmotion.x;
        p[1]=e.xmotion.y;
        if (b)
            draw_outline(c);
        if (resize)
            set_size(c, p);
        else
            set_position(c, original, start, p);
        (b ? draw_outline : jbwm_move_resize)(c);
    }
}
/* Drag the specified client.  Resize the client if resize is true.  */
void jbwm_drag(struct JBWMClient * restrict c, const bool resize)
{
    Display * d = c->screen->display;
    XRaiseWindow(d, c->parent);
    if (resize && (c->opt.no_resize || c->opt.shaded))
        return;
    grab_pointer(d, c->screen->xlib->root);
    if (resize) {
        union JBWMRectangle * restrict g = &c->size;
        jbwm_warp(d, c->window, g->width, g->height);
    }
    drag_event_loop(c, resize);
    if (c->opt.border)
        draw_outline(c);
    XUngrabPointer(d, CurrentTime);
    jbwm_move_resize(c);
}

