
#include "jbwm.h"
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
	Display *d;
	Window w;

#ifdef USE_SHAPE
	if(is_shaped(c))
		return;
#endif /* USE_SHAPE */
	d=jbwm.X.dpy;
	w = c->titlebar = XCreateSimpleWindow(d, c->parent, 0, 0, 
		c->geometry.width, TDIM, 0, 0, 0); 
	XSelectInput(d, w, ExposureMask);
	XSetWindowBackground(d, w, c->screen->bg.pixel);
	XMapRaised(d, w);
	jbwm_grab_button(w, 0, AnyButton);
}

static unsigned int
draw_title(Client * c, char *name)
{
	const size_t name_length = name ? strlen(name) : 0;
	const int x = TDIM + 8;
#ifdef USE_XPm
	const int y = 2*TDIM/3;
#else /* USE_XPM */
	const int y = jbwm.X.font->ascent-JBWM_BORDER;
#endif
	Display *d;
	Window w;

	d=jbwm.X.dpy;
	w = c->titlebar;
	if(!name)
		return 0;
#ifdef USE_XFT
	{
		/* Prevent the text from going over the resize button.  */
		const unsigned short max_width = c->geometry.width - 3 
			* TDIM;
		const ubyte s=c->screen->screen;
		XGlyphInfo e;
		XftDraw *xd;
		XftColor color;
		Visual *v;
		Colormap cm;
		
		XftTextExtentsUtf8(d, jbwm.X.font, (XftChar8 *) name, 
			name_length, &e);
		v=DefaultVisual(d, s);
		cm=DefaultColormap(d, s);
		xd=XftDrawCreate(d, w, v, cm);
		XftColorAllocName(d, v, cm, DEF_FG, &color);
		XftDrawStringUtf8(xd, &color, jbwm.X.font, x, y, 
			(XftChar8 *) name, e.width > max_width && e.width > 0 
			? name_length * max_width / e.width : name_length);
		XftDrawDestroy(xd);
		XftColorFree(d, v, cm, &color);
	}
#else /* ! USE_XFT */
	{
		GC gc;
		XGCValues v;
		XFontStruct *f;
		
		f=jbwm.X.font;
		v.font=f->fid;
		v.foreground=c->screen->fg.pixel;
		gc=XCreateGC(d, w, GCFont | GCForeground, &v);
		XDrawString(d, w, gc, x, y, name, 
			name_length);
		XFreeGC(d, gc);
	}
#endif /* USE_XFT */
	XFree(name);

	return name_length;
}

static void
draw_titlebar(Client * c, char *name)
{
	const unsigned short width = c->geometry.width;
	const unsigned short resize_offset = width - TDIM;
	const unsigned short shade_offset = resize_offset - TDIM;
	XRectangle g;

	XClearWindow(jbwm.X.dpy, c->titlebar);
#ifndef USE_XPM
	g.x=g.y=0;
	g.width=g.height=TDIM;
	draw(c->titlebar, &g, TITLEBAR_CLOSE_BG);
	g.x=shade_offset;
	draw(c->titlebar, &g, TITLEBAR_SHADE_BG);
	g.x=resize_offset;
	draw(c->titlebar, &g, TITLEBAR_RESIZE_BG);
#else /* USE_XPM */
	g.x=g.y=0;
	g.width=g.height=TDIM;
	draw_xpm(c->titlebar, &g, close_button_xpm);
	g.x=TDIM;
	do {
		draw_xpm(c->titlebar, &g, gradient_xpm);
		g.x+=1024;
	} while(g.x<c->geometry.width);
	g.x=shade_offset;
	draw_xpm(c->titlebar, &g, shade_xpm);
	g.x=resize_offset;
	draw_xpm(c->titlebar, &g, resize_button_xpm);
#endif /* !USE_XPM */
	draw_title(c, name);
}

void
update_titlebar(Client * c)
{
	Window iw = c->titlebar;

#ifdef USE_SHAPE
	set_shape(c);
#endif /* USE_SHAPE */
	if(!iw)
	{
		new_titlebar(c);
		/* Return here to prevent BadWindow/BadDrawable errors */
		return;
	}

	/* Client specific data.  */
	XMoveResizeWindow(jbwm.X.dpy, iw, 0, 0, c->geometry.width,
		TDIM);
	/* Depending on common data.  */
	{
		char *name;

		XFetchName(jbwm.X.dpy, c->window, &name);
		draw_titlebar(c, name);
	}
}

void
delete_titlebar(Client * c)
{
	if(c && c->titlebar)
		XDestroyWindow(jbwm.X.dpy, c->titlebar);
	c->titlebar = None;
}

