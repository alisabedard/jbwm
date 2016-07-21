// Copyright 2016, Jeffrey E. Bedard
#include "xcb.h"

#include "log.h"
#include "util.h"

#include <string.h>

__attribute__((noreturn,nonnull(1)))
static void xerr(xcb_connection_t * x, const char * msg)
{
        xcb_disconnect(x);
	perror(msg);
	exit(1);
}

xcb_connection_t * jb_get_xcb_connection(const char * display, int * screen)
{
        xcb_connection_t * x = xcb_connect(display, screen);
        switch(xcb_connection_has_error(x)) {
        case 0: // Success
                break;
        case XCB_CONN_ERROR:
                xerr(x, "X transport error, DISPLAY unavailable");
        case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
                xerr(x, "Extension not supported");
        case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
                xerr(x, "Insufficient memory");
        case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
                xerr(x, "Request length exceeded");
        case XCB_CONN_CLOSED_PARSE_ERR:
                xerr(x, "Invalid DISPLAY string");
        case XCB_CONN_CLOSED_INVALID_SCREEN:
                xerr(x, "Invalid screen");
        }
	return x;
}

xcb_screen_t * jb_get_xcb_screen(xcb_connection_t * x)
{
        return xcb_setup_roots_iterator(xcb_get_setup(x)).data;
}

pixel_t jb_get_pixel(xcb_connection_t * x, const xcb_colormap_t cmap,
	const char * color)
{
	xcb_alloc_named_color_cookie_t c
	= xcb_alloc_named_color(x, cmap, strlen(color), color);
	xcb_alloc_named_color_reply_t * r
	= xcb_alloc_named_color_reply(x, c, NULL);
	if (jb_check(r, "Could not allocate color"))
		return 0;
	pixel_t p = r->pixel;
	free(r);
	return p;
}

pixel_t jb_get_rgb_pixel(xcb_connection_t * x, const xcb_colormap_t cm,
	const int16_t r, const int16_t g, const int16_t b)
{
	xcb_alloc_color_cookie_t c = xcb_alloc_color(x, cm, r, g, b);
	xcb_alloc_color_reply_t * rpl = xcb_alloc_color_reply(x, c, NULL);
	if(jb_check(rpl, "Could not allocate RGB color"))
		return 0;
	const pixel_t p = rpl->pixel;
	free(rpl);
	return p;
}

pixel_t jb_set_fg(xcb_connection_t * x, const xcb_gc_t gc, const pixel_t p)
{
	xcb_change_gc(x, gc, XCB_GC_FOREGROUND, &(uint32_t){p});
	return p;
}

pixel_t jb_set_bg(xcb_connection_t * x, const xcb_gc_t gc, const pixel_t p)
{
	xcb_change_gc(x, gc, XCB_GC_BACKGROUND, &(uint32_t){p});
	return p;
}

