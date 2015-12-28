
#include "jbwm.h"
#include <string.h>
/* Include the title button bitmaps.  */
#ifdef USE_XPM
#include <X11/xpm.h>
#include "images/close_button.xpm"
#include "images/resize_button.xpm"
#include "images/gradient.xpm"
#include "images/shade.xpm"
#endif /* USE_XPM */

#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif /* USE_XFT */

static void
new_titlebar(Client *c)
{
	if(c->flags&JB_CLIENT_NO_TB)
		return;
#ifdef USE_SHAPE
	if(is_shaped(c))
		return;
#endif /* USE_SHAPE */
	const Window w = c->titlebar = XCreateSimpleWindow(D, c->parent, 0, 0, 
		c->size.width, TDIM, 0, 0, 0); 
	XSelectInput(D, w, ExposureMask);
	XColor color=jbwm_color(TITLEBAR_BG);
	XSetWindowBackground(D, w, color.pixel);
	free_color(color);
	XMapRaised(D, w);
	jbwm_grab_button(w, 0, AnyButton);
}

static unsigned int
draw_title(Client * c, char *name)
{
	const int x = TDIM + 8;
#ifdef USE_XPM
	const int y = 2*TDIM/3;
#else//!USE_XPM
	const int y = jbwm.X.font->ascent-JBWM_BORDER;
#endif//USE_XPM
	const Window w = c->titlebar;
	if(!name)
		return 0;
	const size_t name_length = name ? strlen(name) : 0;
#ifdef USE_XFT
	{
		/* Prevent the text from going over the resize button.  */
		const unsigned short max_width = c->size.width - 3 
			* TDIM;
		const ubyte s=c->screen->screen;
		XGlyphInfo e;
		XftDraw *xd;
		XftColor color;
		Visual *v;
		Colormap cm;
		
		XftTextExtentsUtf8(D, jbwm.X.font, (XftChar8 *) name, 
			name_length, &e);
		v=DefaultVisual(D, s);
		cm=DefaultColormap(D, s);
		xd=XftDrawCreate(D, w, v, cm);
		XftColorAllocName(D, v, cm, DEF_FG, &color);
		XftDrawStringUtf8(xd, &color, jbwm.X.font, x, y, 
			(XftChar8 *) name, e.width > max_width && e.width > 0 
			? name_length * max_width / e.width : name_length);
		XftDrawDestroy(xd);
		XftColorFree(D, v, cm, &color);
	}
#else /* ! USE_XFT */
	XFontStruct *f=jbwm.X.font;
	XGCValues v={.font=f->fid, .foreground=c->screen->fg.pixel};
	GC gc=XCreateGC(D, w, GCFont | GCForeground, &v);
	XDrawString(D, w, gc, x, y, name, name_length);
	XFreeGC(D, gc);
#endif /* USE_XFT */
	XFree(name);
	return name_length;
}

static void
draw_titlebar(Client * c, char *name)
{
	const unsigned short width = c->size.width;
	const unsigned short resize_offset = width - TDIM;
	const unsigned short shade_offset = resize_offset - TDIM;
	XRectangle g={.x=0,.y=0,.width=TDIM,.height=TDIM};

	XClearWindow(D, c->titlebar);
	if(!(c->flags&JB_CLIENT_NO_CLOSE))
	{
#ifndef USE_XPM
		draw(c->titlebar, &g, TITLEBAR_CLOSE_BG);
#else//USE_XPM
		draw_xpm(c->titlebar, &g, close_button_xpm);
#endif//!USE_XPM
	}
#ifdef USE_XPM
	g.x=TDIM;
	do {
		draw_xpm(c->titlebar, &g, gradient_xpm);
		g.x+=1024;
	} while(g.x<c->size.width);
#endif//USE_XPM
	if(!(c->flags&JB_CLIENT_NO_MIN))
	{
		g.x=shade_offset;
#ifndef USE_XPM
		draw(c->titlebar, &g, TITLEBAR_SHADE_BG);
#else//USE_XPM
		draw_xpm(c->titlebar, &g, shade_xpm);
#endif//!USE_XPM
	}
	if(!(c->flags&JB_CLIENT_NO_RESIZE))
	{
		g.x=resize_offset;
#ifndef USE_XPM
		draw(c->titlebar, &g, TITLEBAR_RESIZE_BG);
#else//USE_XPM
		draw_xpm(c->titlebar, &g, resize_button_xpm);
#endif//!USE_XPM
	}
	if(!(c->flags&JB_CLIENT_TEAROFF))
		draw_title(c, name);
}

void
update_titlebar(Client * c)
{
	const Window tb = c->titlebar;

	if(c->flags & JB_CLIENT_MAXIMIZED)
	{
		/* May generate BadWindow on subsequent invocations,
		   however the error handler makes such irrelevant.  */
		delete_titlebar(c);
		return;
	}
#ifdef USE_SHAPE
	set_shape(c);
#endif /* USE_SHAPE */
	if(!tb)
	{
		new_titlebar(c);
		/* Return here to prevent BadWindow/BadDrawable errors */
		return;
	}

	/* Expand/Contract the titlebar width as necessary:  */
	XMoveResizeWindow(D, tb, 0, 0, c->size.width, TDIM);
	XTextProperty p;
	XGetTextProperty(D, c->window, &p, XA_WM_NAME);
	draw_titlebar(c, (char *)p.value);
}

void
delete_titlebar(Client * c)
{
	if(c && c->titlebar)
		XDestroyWindow(D, c->titlebar);
	c->titlebar = 0;
}

