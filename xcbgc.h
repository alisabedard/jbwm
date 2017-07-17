// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#ifndef LIBJB_XCBGC_H
#define LIBJB_XCBGC_H
#include <xcb/xproto.h>
struct JB_XGC {
	void * pad;
	xcb_gcontext_t gid;
};
#define XCBGC(gc) (((struct JB_XGC *)gc)->gid)
#endif//!LIBJB_XCBGC_H
