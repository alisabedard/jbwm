#ifdef TITLEBAR

#include "arwm.h"
/* Include the title button bitmaps.  */
#include <X11/Xft/Xft.h>

static void
setup_titlebar(Client * c)
{
	Display * dpy = arwm.X.dpy;
	Window info_window;

	if(c->flags & AR_CLIENT_SHAPED)
		return;

	info_window=c->info_window 
		= XCreateSimpleWindow(dpy, c->parent, 0, 0,
			c->geometry.width, TITLEBAR_HEIGHT,
			TB_BRDR_WDTH, 0, 
			arwm_get_pixel_for_color(TITLEBAR_BG));
	XSelectInput(arwm.X.dpy, info_window, ExposureMask);
	XMapRaised(dpy, info_window);
	arwm_grab_button(info_window, 0, AnyButton);
}

static void
draw_handles(Client * c)
{
	Window w = c->info_window;
	ARWMButton *close, *resize, *shade, *handle;
	const unsigned int width = c->geometry.width;
#define ARWM_HANDLE_OFFSET(x) (width-((TITLEBAR_HEIGHT)*x))

	close=arwm.titlebar.buttons.close;
	resize=arwm.titlebar.buttons.resize;
	shade=arwm.titlebar.buttons.shade;
	handle=arwm.titlebar.buttons.handle;

	close->parent=resize->parent=shade->parent=handle->parent=w;
	close->c=resize->c=shade->c=handle->c=c;
	handle->geometry->width=ARWM_HANDLE_OFFSET(3);
	handle->geometry->x = TITLEBAR_HEIGHT;
	$(handle, draw);
	$(close, draw);
	shade->geometry->x=ARWM_HANDLE_OFFSET(2);
	$(shade, draw); 
	resize->geometry->x=ARWM_HANDLE_OFFSET(1);
	$(resize, draw);
}

static void
arwm_ARWMTitlebarData_destroy(ARWMTitlebarData * titlebar);

static ARWMButton *
initialize_button(const int r, const int g, const int b)
{
	ARWMButton * button;

	button = new_ARWMButton(DefaultRootWindow(arwm.X.dpy));
	$(button, set_color, get_ARWMColor_for(r, g, b));
	button->geometry->height=button->geometry->width
		= TITLEBAR_HEIGHT;
	
	return button;
}

static void
initialize_buttons(ARWMTitlebarData * titlebar)
{
	titlebar->buttons.close=initialize_button(0xae, 0x80, 0x80);
	titlebar->buttons.resize=initialize_button(0x80, 0xb2, 0x80);
	titlebar->buttons.shade=initialize_button(0x80, 0x80, 0xc3);
	titlebar->buttons.handle=initialize_button(0x50, 0x50, 0x50);
	titlebar->buttons.handle->flags|=ARWMBUTTON_IS_CONTAINER;
}

void
arwm_ARWMTitlebarData_init(ARWMTitlebarData * titlebar)
{
	if(titlebar->initialized)
		return;
	initialize_buttons(titlebar);
	titlebar->delete=&arwm_ARWMTitlebarData_destroy;
	titlebar->initialized=True;
}

static void
delete_buttons(ARWMTitlebarData * titlebar)
{
	$(titlebar->buttons.close, delete);
	$(titlebar->buttons.resize, delete);
	$(titlebar->buttons.shade, delete);
	$(titlebar->buttons.handle, delete);
}

/* This assumes that the memory of the ARWMTitlebarData instance
   passed is managed externally.  */
static void
arwm_ARWMTitlebarData_destroy(ARWMTitlebarData * titlebar)
{
	if(!titlebar->initialized)
		return;
	delete_buttons(titlebar);
}

void
update_info_window(Client * c)
{
	if(!c || (c->flags & AR_CLIENT_DONT_USE_TITLEBAR)
		|| (c->flags & AR_CLIENT_DONT_MANAGE))
	{
		remove_info_window(c);
		return;
	}
	else
		set_shape(c);
	if (!c->info_window)
		setup_titlebar(c);
	/* Client specific data.  */
	arwm_ARWMTitlebarData_init(&(arwm.titlebar));
	XFetchName(arwm.X.dpy, c->window, 
		&(arwm.titlebar.buttons.handle->text));
	XMoveResizeWindow(arwm.X.dpy, c->info_window, 0, 0, 
		c->geometry.width,
		TITLEBAR_HEIGHT);	
	draw_handles(c);
}

void
remove_info_window(Client * c)
{
	if (c && c->info_window && arwm.X.dpy)
          {
            LOG_DEBUG("XDestroyWindow()\n");
		XDestroyWindow(arwm.X.dpy, c->info_window);
          }
        LOG_DEBUG("DONT Set INFO_WINDOW to None\n");
	//c->info_window = None;
}

#endif /* TITLEBAR */

