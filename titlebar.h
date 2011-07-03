#ifndef ARWM_TITLEBAR_H
#define ARWM_TITLEBAR_H

#define TB_BRDR_WDTH 0

#include "graphics.h"

typedef struct _ARWMTitlebarData
{
	Bool initialized;

	struct
	{
		ARWMButton *close, *resize, *shade, *handle;
	} buttons;

#define ARWM_TITLEBAR_FOREGROUND "white"

	void (*delete)(struct _ARWMTitlebarData *);

} ARWMTitlebarData;

void
arwm_ARWMTitlebarData_init(ARWMTitlebarData * titlebar);

void
update_info_window(Client * c);

void 
remove_info_window(Client * c);

void
arwm_draw_close_button(Client * c);

#define AR_BUTTON_WIDTH TITLEBAR_HEIGHT
#define AR_BUTTON_HEIGHT TITLEBAR_HEIGHT
#define AR_RESIZE_BUTTON_WIDTH AR_BUTTON_WIDTH
#define AR_RESIZE_DELTA (AR_RESIZE_BUTTON_WIDTH+TITLEBUTTON_PADDING)
#define AR_SHADE_DELTA (AR_RESIZE_DELTA+AR_BUTTON_WIDTH+TITLEBUTTON_PADDING)

#endif /* ARWM_TITLEBAR_H */

