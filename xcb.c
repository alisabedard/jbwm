// Copyright 2016, Jeffrey E. Bedard
#include "xcb.h"
// Include here to check syntax:
#include "JBDim.h"
#include "log.h"
#include "util.h"
#include <errno.h>
#include <string.h>
#include <sys/select.h>
bool jb_xcb_cookie_has_error(xcb_connection_t * x, const xcb_void_cookie_t c)
{
	xcb_generic_error_t * e = xcb_request_check(x, c);
	if (!e)
		return false;
	free(e);
	return true;
}
static void xerr(xcb_connection_t * x, const char * msg)
{
	xcb_disconnect(x);
	if (errno)
		perror(msg);
	else
		fprintf(stderr, "%s\n", msg);
	exit(1);
}
void jb_check_x(xcb_connection_t * x)
{
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
}
xcb_connection_t * jb_get_xcb_connection(const char * display, int * screen)
{
	errno = 0;
	xcb_connection_t * x = xcb_connect(display, screen);
	jb_check_x(x);
	return x;
}
xcb_screen_t * jb_get_xcb_screen(xcb_connection_t * x)
{
	return xcb_setup_roots_iterator(xcb_get_setup(x)).data;
}
pixel_t jb_get_pixel(xcb_connection_t * x, const xcb_colormap_t cmap,
	const char * color)
{
	xcb_alloc_named_color_cookie_t c = xcb_alloc_named_color(x,
		cmap, strlen(color), color);
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
static pixel_t jb_set_named_color(xcb_connection_t * xc, const xcb_gc_t gc,
	const char * color, const bool is_fg)
{
	return (is_fg ? jb_set_fg : jb_set_bg)(xc, gc,
		jb_get_pixel(xc, jb_get_default_colormap(xc), color));
}
pixel_t jb_set_named_fg(xcb_connection_t * xc, const xcb_gc_t gc,
	const char * color)
{
	return jb_set_named_color(xc, gc, color, true);
}
pixel_t jb_set_named_bg(xcb_connection_t * xc, const xcb_gc_t gc,
	const char * color)
{
	return jb_set_named_color(xc, gc, color, false);
}
xcb_atom_t jb_get_atom(xcb_connection_t * x, const char * name)
{
	xcb_intern_atom_reply_t * r = xcb_intern_atom_reply(x,
		xcb_intern_atom(x, false, strlen(name), name), NULL);
	const xcb_atom_t a = r->atom;
	free(r);
	return a;
}
static xcb_colormap_t get_cmap_from_winattr(xcb_connection_t * xc,
	xcb_get_window_attributes_cookie_t wac)
{
	xcb_get_window_attributes_reply_t * r
		= xcb_get_window_attributes_reply(xc, wac, NULL);
	jb_require(r, "Could not get colormap information.");
	const xcb_colormap_t cm = r->colormap;
	free(r);
	return cm;
}
xcb_colormap_t jb_get_colormap(xcb_connection_t * xc,
	const xcb_window_t win)
{
	return get_cmap_from_winattr(xc,
		xcb_get_window_attributes(xc, win));
}
xcb_colormap_t jb_get_default_colormap(xcb_connection_t * xc)
{
	return jb_get_xcb_screen(xc)->default_colormap;
}
/* Create a gc with foreground and background as specified.
   If gc is passed as 0, a new gc value is generated and returned.  */
xcb_gc_t jb_create_gc(xcb_connection_t * xc, xcb_gc_t gc,
	const xcb_window_t win, const char * restrict fg,
	const char * restrict bg)
{
	// defer processing reply
	const xcb_get_window_attributes_cookie_t wac
		= xcb_get_window_attributes(xc, win);
	if (!gc)
		gc = xcb_generate_id(xc);
	{
		xcb_void_cookie_t c
			= xcb_create_gc_checked(xc, gc, win, 0, NULL);
		xcb_generic_error_t * e = xcb_request_check (xc, c);
		if (jb_check(!e, "Could not create gc"))
			free(e);
	}
	const xcb_colormap_t cm = get_cmap_from_winattr(xc, wac);
	const pixel_t fgpx = jb_get_pixel(xc, cm, fg);
	const pixel_t bgpx = jb_get_pixel(xc, cm, bg);
	xcb_change_gc(xc, gc, XCB_GC_FOREGROUND | XCB_GC_BACKGROUND,
		(uint32_t[]){fgpx, bgpx});
	return gc;
}
bool jb_next_event_timed(xcb_connection_t * x,
	xcb_generic_event_t ** e, const uint32_t delay)
{
	jb_check_x(x);
	if ((*e = xcb_poll_for_event(x)))
		return true;
	int fd = xcb_get_file_descriptor(x);
	fd_set r;
	FD_ZERO(&r);
	FD_SET(fd, &r);
	if (!select(fd + 1, &r, NULL, NULL, &(struct timeval){
		.tv_usec = delay}))
		return false; // timeout
	// event occurred before timeout:
	*e = xcb_poll_for_event(x);
	return true;
}
// Open font specified by name.  initialized fid must be supplied
bool jb_open_font(xcb_connection_t * xc, xcb_font_t fid,
	const char * name)
{
	xcb_void_cookie_t c = xcb_open_font_checked(xc, fid,
		strlen(name), name);
	if (jb_xcb_cookie_has_error(xc, c)) {
		LIBJB_WARN("Failed to load font: %s", name);
		return false;
	}
	return true;
}
void jb_set_window_name(xcb_connection_t * xc, const xcb_window_t win,
	char * name)
{
	xcb_change_property(xc, XCB_PROP_MODE_REPLACE, win,
		XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
		strlen(name), name);
}
void jb_set_icon_name(xcb_connection_t * xc, const xcb_window_t win,
	char * name)
{
	xcb_change_property(xc, XCB_PROP_MODE_REPLACE, win,
		XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING, 8,
		strlen(name), name);
}
