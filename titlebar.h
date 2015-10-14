#ifndef JBWM_TITLEBAR_H
#define JBWM_TITLEBAR_H

#include "graphics.h"

typedef struct _JBWMTitlebarData
{
	Bool initialized;

	struct
	{
		JBWMButton *close, *resize, *handle, *shade;
	} buttons;

#ifdef USE_XFT
	struct
	{
		XftDraw *draw;
		XftColor fg;
	} xft;
#endif				/* USE_XFT */
#ifdef USE_XPM
	XImage *close, *resize, *close_inactive, *handle, *shade;
#endif				/* USE_XPM */
} JBWMTitlebarData;

void JBWMTitlebarData_delete(JBWMTitlebarData * titlebar);

void jbwm_JBWMTitlebarData_init(JBWMTitlebarData * titlebar);

void update_info_window(Client * c);

void remove_info_window(Client * c);

void jbwm_draw_close_button(Client * c);

#define JB_BUTTON_WIDTH TITLEBAR_HEIGHT
#define JB_BUTTON_HEIGHT TITLEBAR_HEIGHT
#ifdef USE_XPM
#define JB_RESIZE_BUTTON_WIDTH 24
#else
#define JB_RESIZE_BUTTON_WIDTH JB_BUTTON_WIDTH
#endif
#define JB_RESIZE_DELTA (JB_RESIZE_BUTTON_WIDTH+JBWM_BORDER_WIDTH)
#define JB_SHADE_DELTA (JB_RESIZE_DELTA+JB_BUTTON_WIDTH+JBWM_BORDER_WIDTH)

#endif /* JBWM_TITLEBAR_H */
