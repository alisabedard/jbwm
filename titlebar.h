/* jbwm, Copyright 2016 Jeffrey E. Bedard */
#ifndef JBWM_TITLEBAR_H
#define JBWM_TITLEBAR_H

#include "graphics.h"

void 
update_titlebar(Client * c);

/* Ensure that the title text fits within the titlebar.  */
#ifdef USE_TBAR
#define TDIM (jbwm.X.font->ascent+jbwm.X.font->descent)
#else /* !USE_TBAR */
#define TDIM 16
#endif

#endif /* JBWM_TITLEBAR_H */
