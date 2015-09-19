
#include "jbwm.h"
/* Include the title button bitmaps.  */
#ifdef USE_XPM
#include <X11/xpm.h>
#include "close_button.xpm"
#include "close_button_inactive.xpm"
#include "resize_button.xpm"
#include "gradient.xpm"
#include "shade.xpm"
#elif USE_XBM
#include "close.xbm"
#include "resize.xbm"
#include "shade.xbm"
#endif /* USE_XPM */

#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif /* USE_XFT */

#include "ARWMButton.h"

static void
setup_titlebar(Client * c)
{
	Display *dpy = jbwm.X.dpy;
	Window info_window;

	if(c->flags & AR_CLIENT_SHAPED)
		return;

	info_window = c->info_window = XCreateSimpleWindow(dpy, c->parent, 
		0, 0, c->geometry.width, TITLEBAR_HEIGHT, 0, 0, 0);
	XSelectInput(jbwm.X.dpy, info_window, ExposureMask);
	XMapRaised(dpy, info_window);
	jbwm_grab_button(info_window, 0, AnyButton);
}


static unsigned int
draw_info_strings(Client * c, char *name)
{
	const size_t name_length = name?strlen(name):0;
	const int x = BUTTON_WIDTH + 2 * ARWM_BORDER_WIDTH;
	Window info_window = c->info_window;

	if(!name)
		return 0;
#ifdef USE_XFT
	XftDrawChange(jbwm.titlebar.xft.draw, info_window);
	{
		/* Prevent the text from going over the resize button.  */
		const unsigned short max_width = c->geometry.width - 3 
			* TITLEBAR_HEIGHT;
		XGlyphInfo e;

		XftTextExtentsUtf8(jbwm.X.dpy, jbwm.X.font, (XftChar8 *) name, 
				name_length, &e);
		XftDrawChange(jbwm.titlebar.xft.draw, info_window);
		XftDrawStringUtf8(jbwm.titlebar.xft.draw, 
				&(jbwm.titlebar.xft.fg), 
				jbwm.X.font, x+4, (TITLEBAR_HEIGHT-e.y) +e.y/2, 
				(XftChar8 *) name, e.width > max_width 
				&& e.width > 0 ? name_length * max_width 
				/ e.width : name_length);
	}
#else /* ! USE_XFT */
	XDrawString(jbwm.X.dpy, info_window, c->screen->gc, x+5, 
			TITLE_FONT_HEIGHT*1.25-FONT_Y_OFFSET+2, 
			name, name_length);
#endif /* USE_XFT */
	XFree(name);

	return name_length;
}

static void
draw_button(ARWMButton * button, Window w, const short x)
{
	button->parent=w;
	button->geometry.x = x;
	ARWMButton_draw(button);
}

#ifdef USE_XPM
void
jbwm_draw_close_button(Client * c)
{
	ARWMButton *close = jbwm.titlebar.buttons.close;

#ifdef TITLEBAR_DEBUG
	LOG_DEBUG("jbwm_draw_close_button(c)\n");
	LOG_DEBUG("x:%d\ty:%d\tw:%d\th:%d\n", close->geometry->x,
		close->geometry->y, close->geometry->width,
		close->geometry->height);
#endif /* TITLEBAR_DEBUG */
	close->image =
		(c->flags & AR_CLIENT_ACTIVE) ? jbwm.titlebar.
		close : jbwm.titlebar.close_inactive;
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
	ARWMButton *handle;
	const unsigned short width = c->geometry.width;
	const unsigned short resize_offset = width - AR_RESIZE_DELTA;
#ifdef USE_SHADE
	const unsigned short shade_offset = width - AR_SHADE_DELTA;
#endif /* USE_SHADE */

	if(c->flags & AR_CLIENT_SHAPED)
		return;
	handle = jbwm.titlebar.buttons.handle;

#ifndef USE_XBM
	handle->geometry.width = width;
	draw_button(handle, w, 0);
#endif /* !USE_XBM */
#ifdef USE_CLOSE_BUTTON
	jbwm_draw_close_button(c);
#endif /* USE_CLOSE_BUTTON */
	draw_info_strings(c, name);
#ifdef USE_SHADE
	draw_button(jbwm.titlebar.buttons.shade, w, shade_offset);
#endif /* USE_SHADE */
	draw_button(jbwm.titlebar.buttons.resize, w, resize_offset);
}

#if defined(USE_XPM) || defined(USE_XBM)
static void
initialize_images(ARWMTitlebarData * titlebar)
{

#ifdef USE_XPM
	Display *dpy = jbwm.X.dpy;

#ifdef TITLEBAR_DEBUG
	LOG_DEBUG("Load pixmaps...");
#endif /* TITLEBAR_DEBUG */
#define XPMIMAGE(xpm, dest) XpmCreateImageFromData(dpy, xpm##_xpm,\
	&(titlebar->dest), NULL, NULL)
	XPMIMAGE(gradient, handle);
	XPMIMAGE(resize_button, resize);
	XPMIMAGE(close_button, close);
	XPMIMAGE(close_button_inactive, close_inactive);
#ifdef USE_SHADE
	XPMIMAGE(shade, shade);
#endif /* USE_SHADE */
#elif USE_XBM
#define XBMIMAGE(i) titlebar->i = jbwm_get_XImage_for_XBM(\
	i##_bits, i##_width, i##_height)
	XBMIMAGE(resize);
	XBMIMAGE(close);
#ifdef USE_SHADE
	XBMIMAGE(shade);
#endif /* USE_SHADE */
#endif /* USE_XPM */
}
#endif /* USE_XPM || USE_XBM */

static void
initialize_buttons(ARWMTitlebarData * titlebar, Display * dpy)
{
	Window root = DefaultRootWindow(dpy);
#ifndef USE_XBM
	ARWMButton *handle;
#endif /* ! USE_XBM */

#if defined(USE_XPM) || defined(USE_XBM)
#define IMG(item) titlebar->item
#ifdef USE_XBM
#define IMGDIM(item)\
	item->image_height = item##_height;\
	item->image_width= item##_width;
#else /* !USE_XBM */
#define IMGDIM(item)
#endif /* USE_XBM */
#else /* USE_XBM || USE_XPM */
#define IMG(item) NULL
#define IMGDIM(item)
#endif /* USE_XBM */

#define TBUTTON(item, bg) titlebar->buttons.item = ARWMButton_new(\
	root, bg, TITLEBAR_HEIGHT,\
	TITLEBAR_HEIGHT, IMG(item)); IMGDIM(item)
#define RGB_TBUTTON(item, bg) TBUTTON(item, jbwm_new_gc(bg))
#define XCOLOR_TBUTTON(item, bg) TBUTTON(item, jbwm_new_gc_for_XColor(bg))
	RGB_TBUTTON(close, TITLEBAR_CLOSE_BG);
	RGB_TBUTTON(resize, TITLEBAR_RESIZE_BG);
#ifdef USE_SHADE
	RGB_TBUTTON(shade, TITLEBAR_SHADE_BG);
#endif /* USE_SHADE */
#ifndef USE_XBM
	handle=XCOLOR_TBUTTON(handle, jbwm.X.screens->bg);
	handle->span_image=True;
#endif /* ! USE_XBM */
}

#ifdef USE_XFT
static void
initialize_font_data(ARWMTitlebarData * titlebar, Display * dpy)
{
	const int scr = DefaultScreen(dpy);
	Visual *visual = DefaultVisual(dpy, scr);
	Colormap colormap = DefaultColormap(dpy, scr);

	XftColorAllocName(dpy, visual, colormap, opt.color.fg,
		&(titlebar->xft.fg));
	titlebar->xft.draw =
		XftDrawCreate(dpy, DefaultRootWindow(dpy), visual,
		colormap);
}
#endif /* USE_XFT */

void
jbwm_ARWMTitlebarData_init(ARWMTitlebarData * titlebar)
{
	Display *dpy = jbwm.X.dpy;

	if(titlebar->initialized)
		return;

#if defined(USE_XPM) || defined(USE_XBM)
	initialize_images(titlebar);
#endif /* USE_XPM || USE_XBM */
	initialize_buttons(titlebar, dpy);
#ifdef USE_XFT
	initialize_font_data(titlebar, dpy);
#endif /* USE_XFT */
	titlebar->initialized = True;
}

static void
delete_buttons(ARWMTitlebarData * titlebar)
{
	ARWMButton_delete(titlebar->buttons.close);
	ARWMButton_delete(titlebar->buttons.resize);
	ARWMButton_delete(titlebar->buttons.shade);
#ifdef USE_XPM
	ARWMButton_delete(titlebar->buttons.handle);
#endif /* USE_XPM */
}

#ifdef USE_XPM
static void
free_XImage(ARWMTitlebarData * t)
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
free_Xft_data(ARWMTitlebarData * titlebar)
{
	XftDraw *draw = titlebar->xft.draw;

	XftColorFree(jbwm.X.dpy, XftDrawVisual(draw),
		XftDrawColormap(draw), &(titlebar->xft.fg));
	XftDrawDestroy(draw);
}
#endif /* USE_XFT */

/* This assumes that the memory of the ARWMTitlebarData instance
   passed is managed externally.  */
void
ARWMTitlebarData_delete(ARWMTitlebarData * titlebar)
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
	XMoveResizeWindow(jbwm.X.dpy, iw, 0, 0, 
			c->geometry.width, TITLEBAR_HEIGHT);
	XClearWindow(jbwm.X.dpy, iw);
	/* Depending on common data.  */
	jbwm_ARWMTitlebarData_init(&(jbwm.titlebar));
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
