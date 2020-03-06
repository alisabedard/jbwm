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
__attribute__((nonnull))
static inline void query_pointer(Display * dpy, Window w,
    int16_t * p)
{
    int x, y;
    int d; /*  dummy integer */
    unsigned int u; /*  dummy unsigned integer */
    XQueryPointer(dpy, w, &w, &w, &d, &d, &x, &y, &u);
    p[0] = x;
    p[1] = y;
}
__attribute__((nonnull))
static void draw_outline(struct JBWMClient * c)
{
    if(c->opt.border){
        uint8_t const fh=c->screen->font_height, o = (c->opt.no_title_bar^1)*fh;
        const union JBWMRectangle * g = &c->size;
        enum { BORDER = 1 };
        Display * d = c->screen->xlib->display;
        XDrawRectangle(d, c->screen->xlib->root, c->screen->border_gc,
            g->x, g->y - o, g->width + BORDER, g->height + BORDER + o);
    } else {
        jbwm_move_resize(c);
    }
}
static void drag_event_loop(struct JBWMClient * c,
    struct JBWMClient ** head_client, int16_t * start, bool const resize)
{
    Display *d;
    union JBWMRectangle * g;
    XEvent e;
    /***/
    d=c->screen->xlib->display;
    g = &c->size;
    for(;;){
        XMaskEvent(d, JBWMMouseMask, &e);
        if(e.type==MotionNotify){
            int16_t p[]={e.xmotion.x,e.xmotion.y};
            draw_outline(c);
            if(resize){
                /* Compute distance.  */
                p[0]=abs(g->array[0]-p[0]);
                p[1]=abs(g->array[1]-p[1]);
                /* Avoid client crashes if the axis geometry is 0.  */
                g->array[2]=p[0]>0?p[0]:1;
                g->array[3]=p[1]>0?p[1]:1;
            } else { /* move */
                p[0]+=start[0];
                p[1]+=start[1];
                g->array[0]=p[0];
                g->array[1]=p[1];
                jbwm_snap_client(c, head_client);
            }
            draw_outline(c); /* Erase the previous outline.  */
            /* Test that this function remains an atomic call by
               using its recursive definition.
               drag_event_loop(c, start, resize);  */
        } else
            break;
    }
}
/* Drag the specified client.  Resize the client if resize is true.  */
void jbwm_drag(struct JBWMClient * c, struct JBWMClient ** head_client,
    const bool resize)
{
    int16_t start[2];
    Display * d = c->screen->xlib->display;
    Window const r = c->screen->xlib->root;
    union JBWMRectangle * g = &c->size;
    Cursor const cursor = XCreateFontCursor(d, XC_fleur);
    XRaiseWindow(d, c->parent);
    if (!(resize && (c->opt.no_resize || c->opt.shaded))){
        XGrabPointer(d, r, false, JBWMMouseMask, GrabModeAsync,
            GrabModeAsync, None, cursor, CurrentTime);
        if (resize){
            jbwm_warp(d, c->window, g->width, g->height);
        }
        query_pointer(d, r, start);
        /* Adjust for the current window position to find the offset.  */
        start[0]=g->array[0]-start[0];
        start[1]=g->array[1]-start[1];
        drag_event_loop(c, head_client, start, resize);
        draw_outline(c);
        XUngrabPointer(d, CurrentTime);
        jbwm_move_resize(c);
    }
}

