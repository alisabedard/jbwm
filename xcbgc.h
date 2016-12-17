// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#ifndef JBWM_XCBGC_H
#define JBWM_XCBGC_H
#include <xcb/xproto.h>
struct JB_XGC {
	void * pad;
	xcb_gcontext_t gid;
};
#define XCBGC(gc) (((struct JB_XGC *)gc)->gid)
#endif//!JBWM_XCBGC_H
