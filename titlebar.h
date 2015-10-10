#ifndef ARWM_TITLEBAR_H
#define ARWM_TITLEBAR_H

#include "graphics.h"

typedef struct _ARWMTitlebarData
{
	Bool initialized;

	struct
	{
		ARWMButton *close, *resize, *handle;
#ifdef USE_SHADE
		ARWMButton *shade;
#endif
	} buttons;

#ifdef USE_XFT
	struct
	{
		XftDraw *draw;
		XftColor fg;
	} xft;
#endif				/* USE_XFT */
#ifdef USE_XPM
	XImage *close, *resize;
	XImage *close_inactive, *handle;
#ifdef USE_SHADE
	XImage *shade;
#endif				/* USE_SHADE */
#endif				/* USE_XPM */
} ARWMTitlebarData;

void ARWMTitlebarData_delete(ARWMTitlebarData * titlebar);

void jbwm_ARWMTitlebarData_init(ARWMTitlebarData * titlebar);

void update_info_window(Client * c);

void remove_info_window(Client * c);

void jbwm_draw_close_button(Client * c);

#define AR_BUTTON_WIDTH TITLEBAR_HEIGHT
#define AR_BUTTON_HEIGHT TITLEBAR_HEIGHT
#ifdef USE_XPM
#define AR_RESIZE_BUTTON_WIDTH 24
#else
#define AR_RESIZE_BUTTON_WIDTH AR_BUTTON_WIDTH
#endif
#define AR_RESIZE_DELTA (AR_RESIZE_BUTTON_WIDTH+ARWM_BORDER_WIDTH)
#define AR_SHADE_DELTA (AR_RESIZE_DELTA+AR_BUTTON_WIDTH+ARWM_BORDER_WIDTH)

#endif /* ARWM_TITLEBAR_H */
