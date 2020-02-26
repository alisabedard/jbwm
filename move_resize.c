// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#include "move_resize.h"
#include "JBWMClient.h"
#include "font.h"
#include "log.h"
#include "mwm.h"
#include "shape.h"
#include "title_bar.h"
static void jbwm_configure_client(struct JBWMClient * restrict c)
{
    if (!c) // prevent segmentation fault
        return;
    const Window w = c->window;
    union JBWMRectangle * restrict g = &c->size;
    XSendEvent(c->screen->display, w, true, StructureNotifyMask, (XEvent
            *) &(XConfigureEvent){.x = g->x, .y = g->y, .width = g->width,
        .height = g->height, .type = ConfigureNotify, .event = w,
        .window = w, .above = c->parent, .override_redirect = true,
        .border_width = c->opt.border});
}
static void do_move(Display * d, const Window parent,
    const Window window, union JBWMRectangle * restrict sz,
    const uint8_t offset)
{
    XMoveResizeWindow(d, parent, sz->x, sz->y - offset,
        sz->width, sz->height + offset);
    XMoveResizeWindow(d, window, 0, offset,
        sz->width, sz->height);
}
void jbwm_move_resize(struct JBWMClient * restrict c)
{
    struct JBWMClientOptions * restrict o = &c->opt;
    const uint8_t offset = o->no_title_bar || o->fullscreen
    ? 0 : c->screen->font_height;
    if(offset) { // Leave braces in case title bar support was disabled.
        jbwm_handle_mwm_hints(c);
        jbwm_update_title_bar(c);
    } // Skip shaped and fullscreen clients.
    union JBWMRectangle * restrict s = &c->size;
    do_move(c->screen->display, c->parent, c->window, s, offset);
    jbwm_set_shape(c);
    jbwm_configure_client(c);
}

