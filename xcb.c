// Copyright 2016, Jeffrey E. Bedard
#include "xcb.h"
#include "log.h"
#include "util.h"
// Include this just to check syntax
#include "size.h"
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
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
xcb_atom_t jb_get_atom(xcb_connection_t * x, const char * name)
{
	xcb_intern_atom_reply_t * r = xcb_intern_atom_reply(x,
		xcb_intern_atom(x, false, strlen(name), name), NULL);
	const xcb_atom_t a = r->atom;
	free(r);
	return a;
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
