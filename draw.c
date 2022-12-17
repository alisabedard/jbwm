#include "draw.h"
#include "font.h"
void jbwm_draw_string(struct JBWMScreen *s, Window const target,
  int16_t const x, int16_t const y, char *text, size_t const length){
#ifdef JBWM_USE_XFT
  XftDraw *xd;
  xd = s->xft;
  XftDrawChange(xd, target);
  XftDrawStringUtf8(xd, &s->font_color, jbwm_get_font(),
    x, /*jbwm_get_font_ascent()+*/y, (XftChar8 *)text, length);
#else//!JBWM_USE_XFT
  XDrawString(s->xlib->display,target,s->gc,x,
    /*jbwm_get_font_ascent()+*/y, text, length);
#endif//JBWM_USE_XFT
}
