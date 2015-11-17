
#include "jbwm.h"
/* Include the title button bitmaps.  */
#ifdef USE_XPM
#include <X11/xpm.h>
#include "images/close_button.xpm"
#include "images/close_button_inactive.xpm"
#include "images/resize_button.xpm"
#include "images/gradient.xpm"
#include "images/shade.xpm"
#endif /* USE_XPM */

#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif /* USE_XFT */

#include "JBWMButton.h"

static void
setup_titlebar(Client * c)
{
	Display *dpy = jbwm.X.dpy;
	Window info_window;

#ifdef USE_SHAPE
	if(is_shaped(c))
		return;
#endif /* USE_SHAPE */

	info_window = c->info_window =
		XCreateSimpleWindow(dpy, c->parent, 0, 0,
		c->geometry.width, TITLEBAR_HEIGHT, 0, 0, 0);
	XSelectInput(jbwm.X.dpy, info_window, ExposureMask);
	XMapRaised(dpy, info_window);
	jbwm_grab_button(info_window, 0, AnyButton);
}

static unsigned int
draw_info_strings(Client * c, char *name)
{
	const size_t name_length = name ? strlen(name) : 0;
	const int x = BUTTON_WIDTH + 2 * JBWM_BORDER_WIDTH;
	Window info_window = c->info_window;

	if(!name)
		return 0;
#ifdef USE_XFT
	XftDrawChange(jbwm.titlebar.xft.draw, info_window);
	{
		/* Prevent the text from going over the resize button.  */
		const unsigned short max_width =
			c->geometry.width - 3 * TITLEBAR_HEIGHT;
		XGlyphInfo e;

		XftTextExtentsUtf8(jbwm.X.dpy, jbwm.X.font,
			(XftChar8 *) name, name_length, &e);
		XftDrawChange(jbwm.titlebar.xft.draw, info_window);
		XftDrawStringUtf8(jbwm.titlebar.xft.draw,
			&(jbwm.titlebar.xft.fg), jbwm.X.font, x + 4,
			(TITLEBAR_HEIGHT - e.y) + e.y / 2,
			(XftChar8 *) name, e.width > max_width
			&& e.width >
			0 ? name_length * max_width /
			e.width : name_length);
	}
#else /* ! USE_XFT */
	XDrawString(jbwm.X.dpy, info_window, c->screen->gc, x + 5,
		TITLE_FONT_HEIGHT - FONT_Y_OFFSET, name, name_length);
#endif /* USE_XFT */
	XFree(name);

	return name_length;
}

static void
draw_button(JBWMButton * button, Window w, const short x)
{
	button->parent = w;
	button->geometry.x = x;
	JBWMButton_draw(button);
}

#ifdef USE_XPM
void
jbwm_draw_close_button(Client * c)
{
	JBWMButton *close = jbwm.titlebar.buttons.close;

#ifdef TITLEBAR_DEBUG
	LOG_DEBUG("jbwm_draw_close_button(c)\n");
	LOG_DEBUG("x:%d\ty:%d\tw:%d\th:%d\n", close->geometry->x,
		close->geometry->y, close->geometry->width,
		close->geometry->height);
#endif /* TITLEBAR_DEBUG */
	close->image =
		(c->flags & JB_CLIENT_ACTIVE) ? jbwm.
		titlebar.close : jbwm.titlebar.close_inactive;
	draw_button(close, c->info_window, 0);
}
#else
#define jbwm_draw_close_button(c) draw_button(jbwm.titlebar.buttons.close,\
		c->info_window, 0);
#endif /* USE_XPM */

static void
draw_titlebar(Client * c, char *name)
{
	Window w = c->info_window;
	JBWMButton *handle;
	const unsigned short width = c->geometry.width;
	const unsigned short resize_offset = width - JB_RESIZE_DELTA;
	const unsigned short shade_offset = width - JB_SHADE_DELTA;

#ifdef USE_SHAPE
	if(is_shaped(c))
		return;
#endif /* USE_SHAPE */
	handle = jbwm.titlebar.buttons.handle;

	handle->geometry.width = width;
	draw_button(handle, w, 0);
	jbwm_draw_close_button(c);
	draw_info_strings(c, name);
	draw_button(jbwm.titlebar.buttons.shade, w, shade_offset);
	draw_button(jbwm.titlebar.buttons.resize, w, resize_offset);
}

#ifdef USE_XPM
static void
initialize_images(JBWMTitlebarData * titlebar)
{
	Display *dpy = jbwm.X.dpy;

#define XPMIMAGE(xpm, dest) XpmCreateImageFromData(dpy, xpm##_xpm,\
	&(titlebar->dest), NULL, NULL)
	XPMIMAGE(gradient, handle);
	XPMIMAGE(resize_button, resize);
	XPMIMAGE(close_button, close);
	XPMIMAGE(close_button_inactive, close_inactive);
	XPMIMAGE(shade, shade);
}
#endif /* USE_XPM */

static void
initialize_buttons(JBWMTitlebarData * titlebar, Display * dpy)
{
	Window root = DefaultRootWindow(dpy);
	JBWMButton *handle;

#ifdef USE_XPM
#define IMG(item) titlebar->item
#else
#define IMG(item) NULL
#endif /* USE_XPM */

#define TBUTTON(item, bg) titlebar->buttons.item = JBWMButton_new(\
	root, bg, TITLEBAR_HEIGHT,\
	TITLEBAR_HEIGHT, IMG(item));
#define RGB_TBUTTON(item, bg) TBUTTON(item, jbwm_new_gc(bg))
#define XCOLOR_TBUTTON(item, bg) TBUTTON(item, jbwm_new_gc_for_XColor(bg))
	RGB_TBUTTON(close, TITLEBAR_CLOSE_BG);
	RGB_TBUTTON(resize, TITLEBAR_RESIZE_BG);
	RGB_TBUTTON(shade, TITLEBAR_SHADE_BG);
	handle = XCOLOR_TBUTTON(handle, jbwm.X.screens->bg);
	handle->span_image = true;
}

#ifdef USE_XFT
static void
initialize_font_data(JBWMTitlebarData * titlebar, Display * dpy)
{
	const int scr = DefaultScreen(dpy);
	Visual *visual = DefaultVisual(dpy, scr);
	Colormap colormap = DefaultColormap(dpy, scr);

	XftColorAllocName(dpy, visual, colormap, DEF_FG,
		&(titlebar->xft.fg));
	titlebar->xft.draw =
		XftDrawCreate(dpy, DefaultRootWindow(dpy), visual,
		colormap);
}
#endif /* USE_XFT */

void
jbwm_JBWMTitlebarData_init(JBWMTitlebarData * titlebar)
{
	Display *dpy = jbwm.X.dpy;

	if(titlebar->initialized)
		return;

#ifdef USE_XPM
	initialize_images(titlebar);
#endif /* USE_XPM */
	initialize_buttons(titlebar, dpy);
#ifdef USE_XFT
	initialize_font_data(titlebar, dpy);
#endif /* USE_XFT */
	titlebar->initialized = true;
}

static void
delete_buttons(JBWMTitlebarData * titlebar)
{
	JBWMButton_delete(titlebar->buttons.close);
	JBWMButton_delete(titlebar->buttons.resize);
	JBWMButton_delete(titlebar->buttons.shade);
#ifdef USE_XPM
	JBWMButton_delete(titlebar->buttons.handle);
#endif /* USE_XPM */
}

#ifdef USE_XPM
static void
free_XImage(JBWMTitlebarData * t)
{
	XDestroyImage(t->close);
	XDestroyImage(t->resize);
	XDestroyImage(t->shade);
	XDestroyImage(t->handle);
	XDestroyImage(t->close_inactive);
}
#endif /* USE_XPM */

#ifdef USE_XFT
static void
free_Xft_data(JBWMTitlebarData * titlebar)
{
	XftDraw *draw = titlebar->xft.draw;

	XftColorFree(jbwm.X.dpy, XftDrawVisual(draw),
		XftDrawColormap(draw), &(titlebar->xft.fg));
	XftDrawDestroy(draw);
}
#endif /* USE_XFT */

/* This assumes that the memory of the JBWMTitlebarData instance
   passed is managed externally.  */
void
JBWMTitlebarData_delete(JBWMTitlebarData * titlebar)
{
	if(!titlebar->initialized)
		return;
#ifdef USE_XPM
	free_XImage(titlebar);
#endif /* USE_XPM */
#ifdef USE_XFT
	free_Xft_data(titlebar);
#endif /* USE_XFT */
	delete_buttons(titlebar);
}

void
update_info_window(Client * c)
{
	Window iw = c->info_window;

#ifdef USE_SHAPE
	set_shape(c);
#endif /* USE_SHAPE */
	if(!iw)
	{
		setup_titlebar(c);
		/* Return here to prevent BadWindow/BadDrawable errors */
		return;
	}

	/* Client specific data.  */
	XMoveResizeWindow(jbwm.X.dpy, iw, 0, 0, c->geometry.width,
		TITLEBAR_HEIGHT);
	/* Depending on common data.  */
	jbwm_JBWMTitlebarData_init(&(jbwm.titlebar));
	{
		char *name;

		XFetchName(jbwm.X.dpy, c->window, &name);
		draw_titlebar(c, name);
	}
}

void
remove_info_window(Client * c)
{
	if(c && c->info_window)
		XDestroyWindow(jbwm.X.dpy, c->info_window);
	c->info_window = None;
}
