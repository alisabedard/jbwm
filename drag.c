// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#include "drag.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xproto.h>
#include <X11/Xprotostr.h>
#include <X11/cursorfont.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include "JBWMClient.h"
#include "JBWMClientOptions.h"
#include "JBWMRectangle.h"
#include "JBWMScreen.h"
#include "display.h"
#include "font.h"
#include "screen.h"
#include "snap.h"
enum {
	JBWMMouseMask = ButtonPressMask | ButtonReleaseMask
		| PointerMotionMask
};
static void grab_pointer(Display * d, const Window w)
{
	static Cursor c;
	if (!c)
		c = XCreateFontCursor(d, XC_fleur);
	XGrabPointer(d, w, false, JBWMMouseMask, GrabModeAsync,
		GrabModeAsync, None, c, CurrentTime);
}
static void set_size(struct JBWMClient * restrict c,
	const int16_t * p)
{
	struct JBWMRectangle * g = &c->size;
	g->width = abs(g->x - p[0]);
	g->height = abs(g->y - p[1]);
}
__attribute__((nonnull,pure,warn_unused_result))
static int get_diff(const uint8_t i, const int16_t * original,
	const int16_t * start, const int16_t * p)
{
	return original[i] - start[i] + p[i];
}
static void set_position(struct JBWMClient * restrict c,
	const int16_t * original, const int16_t * start,
	const int16_t * p)
{
	c->size.x = get_diff(0, original, start, p);
	c->size.y = get_diff(1, original, start, p);
	jbwm_snap_client(c);
}
static void do_changes(struct JBWMClient * restrict c, const bool resize,
	const int16_t * start, const int16_t * original,
	const int16_t * p)
{
	if (resize)
		set_size(c, p);
	else // drag
		set_position(c, original, start, p);
}
void jbwm_warp(Display * dpy, const Window w, const int16_t x,
	const int16_t y)
{
	xWarpPointerReq * req;
	GetReq(WarpPointer, req);
	req->srcWid = None;
	req->dstWid = w;
	req->dstX = x;
	req->dstY = y;
}
__attribute__((nonnull))
static void query_pointer(Display * dpy, const Window w,
	int16_t * restrict p)
{
	xQueryPointerReply rep;
	xResourceReq * req;
	GetResReq(QueryPointer, w, req);
	_XReply(dpy, (xReply *)&rep, 0, true);
	p[0] = rep.winX;
	p[1] = rep.winY;
}
__attribute__((nonnull))
static void draw_outline(Display * dpy, struct JBWMClient * restrict c)
{
	const uint8_t o = c->opt.no_title_bar ? 0 : jbwm_get_font_height();
	const struct JBWMScreen * restrict s = jbwm_get_screen(c);
	const struct JBWMRectangle * restrict g = &c->size;
	xPolyRectangleReq * req;
	GetReqExtra(PolyRectangle, sizeof(xRectangle), req);
	req->drawable = s->root;
	req->gc = s->gc->gid;
	xRectangle * rect = (xRectangle *) (req + 1);
	enum { BORDER = 1 };
	*rect = (xRectangle) {g->x, g->y - o,
		g->width + BORDER, g->height + BORDER + o};
}
static void drag_event_loop(struct JBWMClient * restrict c, const bool resize)
{
	Display * d = jbwm_get_display();
	const uint8_t b = c->border;
	const int16_t original[] = {c->size.x, c->size.y};
	int16_t start[2];
	query_pointer(d, jbwm_get_root(c), start);
	for (;;) {
		int16_t p[2];
		{ // e scope
			XEvent e;
			XMaskEvent(d, JBWMMouseMask, &e);
			if (e.type != MotionNotify)
				return;
			p[0] = e.xmotion.x;
			p[1] = e.xmotion.y;
		}
		if (b)
			draw_outline(d, c);
		do_changes(c, resize, start, original, p);
		if (b)
			draw_outline(d, c);
		else
			jbwm_move_resize(c);
	}
}
/* Drag the specified client.  Resize the client if resize is true.  */
void jbwm_drag(struct JBWMClient * restrict c, const bool resize)
{
	Display * d = jbwm_get_display();
	XRaiseWindow(d, c->parent);
	if (resize && (c->opt.no_resize || c->opt.shaded))
		return;
	grab_pointer(d, jbwm_get_root(c));
	if (resize) {
		struct JBWMRectangle * restrict g = &c->size;
		jbwm_warp(d, c->window, g->width, g->height);
	}
	drag_event_loop(c, resize);
	if (c->border)
		draw_outline(d, c);
	XUngrabPointer(d, CurrentTime);
	jbwm_move_resize(c);
}

