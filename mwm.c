// Copyright 2016, Jeffrey E. Bedard <jefbed@gmail.com> 
#include "jbwm.h"

void
handle_mwm_hints(Client * c)
{
        assert(c);
        const Atom mwm_hints = XA("_MOTIF_WM_HINTS");
        struct { unsigned long flags, functions, 
                decor, input_mode, status; } *m;
        unsigned long n;
        if(!(m=get_property(c->window, mwm_hints, mwm_hints, &n)))
                return;

// flags:
#define MWM_HINTS_FUNCTIONS     (1L << 0)
#define MWM_HINTS_DECORATIONS   (1L << 1)
#define MWM_HINTS_INPUT_MODE    (1L << 2)
#define MWM_HINTS_STATUS        (1L << 3)

// functions:
#define MWM_FUNC_ALL            (1L << 0)
#define MWM_FUNC_RESIZE         (1L << 1)
#define MWM_FUNC_MOVE           (1L << 2)
#define MWM_FUNC_MINIMIZE       (1L << 3)
#define MWM_FUNC_MAXIMIZE       (1L << 4)
#define MWM_FUNC_CLOSE          (1L << 5)

// decor:
#define MWM_DECOR_ALL           (1L << 0)
#define MWM_DECOR_BORDER        (1L << 1)
#define MWM_DECOR_RESIZEH       (1L << 2)
#define MWM_DECOR_TITLE         (1L << 3)
#define MWM_DECOR_MENU          (1L << 4)
#define MWM_DECOR_MINIMIZE      (1L << 5)
#define MWM_DECOR_MAXIMIZE      (1L << 6)

// input_mode:
#define MWM_INPUT_MODELESS                      0
#define MWM_INPUT_PRIMARY_APPLICATION_MODAL     1
#define MWM_INPUT_SYSTEM_MODAL                  2
#define MWM_INPUT_FULL_APPLICATION_MODAL        3

// status:
#define MWM_TEAROFF_WINDOW 1
        unsigned long f;
        // If this is set, only use specified decorations:
        if(m->flags & MWM_HINTS_DECORATIONS)
        {
                LOG("MWM_HINTS_DECORATIONS");
                f=m->decor;
                if(f & MWM_DECOR_ALL)
                        goto mwm_decor_all;
                if(!(f & MWM_DECOR_BORDER))
                        c->flags|=JB_NO_BORDER;
                if(!(f & MWM_DECOR_RESIZEH))
                        c->flags|=JB_NO_RESIZE_DECOR;
                if(!(f & MWM_DECOR_TITLE))
                        c->flags|=JB_NO_TB;
                if(!(f & MWM_DECOR_MENU))
                        c->flags|=JB_NO_CLOSE_DECOR;
                if(!(f & MWM_DECOR_MINIMIZE))
                        c->flags|=JB_NO_MIN_DECOR;
                if(!(f & MWM_DECOR_MAXIMIZE))
                        c->flags|=JB_NO_MAX_DECOR;
        }
        if(m->flags & MWM_HINTS_STATUS)
        {
                LOG("MWM_HINTS_STATUS");
                f=m->status;
                if(f & MWM_TEAROFF_WINDOW)
                        c->flags|=JB_TEAROFF;
        }
        if(m->flags & MWM_HINTS_FUNCTIONS)
        {
                LOG("MWM_HINTS_FUNCTIONS");
                f=m->functions;
                if(f & MWM_FUNC_ALL)
                        goto mwm_func_all;
                if(!(f & MWM_FUNC_RESIZE))
                        c->flags|=JB_NO_RESIZE|JB_NO_RESIZE_DECOR;
                if(!(f & MWM_FUNC_MOVE))
                        c->flags|=JB_NO_MOVE;
                if(!(f & MWM_FUNC_CLOSE))
                        c->flags|=JB_NO_CLOSE|JB_NO_CLOSE_DECOR;
                if(!(f & MWM_FUNC_MINIMIZE))
                        c->flags|=JB_NO_MIN|JB_NO_MIN_DECOR;
		if(!(f & MWM_FUNC_MAXIMIZE))
                        c->flags|=JB_NO_MAX|JB_NO_MAX_DECOR;
        }
        if(m->flags & MWM_HINTS_INPUT_MODE)
        {
                LOG("MWM_HINTS_INPUT_MODE");
                if(m->input_mode)
                        c->flags|=JB_MODAL;
        }
mwm_decor_all:
mwm_func_all:
        XFree(m);
        if(c->flags&JB_NO_BORDER)
                c->border=0;
        if(c->flags&JB_TEAROFF)
        {
                c->border=0;
                c->flags|=JB_NO_RESIZE|JB_NO_MIN
                        |JB_NO_MAX;
        }
}

