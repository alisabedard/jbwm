// Copyright 2017, Jeffrey E. Bedard
#include "display.h"
#include <X11/Xproto.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "config.h"
#include "log.h"
#include "util.h"
static void cleanup(const Window w)
{
    struct JBWMClient * restrict c = jbwm_get_client(w);
    if (c) // match found
        jbwm_client_free(c);
}
__attribute__((pure))
static int handle_xerror(Display * d __attribute__((unused)),
    XErrorEvent * restrict e)
{
    switch (e->error_code) {
    case BadAccess:
    if (e->request_code == X_ChangeWindowAttributes)
        jbwm_error("Cannot access the root window");
    break;
    case BadWindow:
    cleanup(e->resourceid);
    return 0;
    case BadAtom:
    return 0;
    }
    JBWM_LOG("XError type:%d xid:%lu serial:%lu"
        " err:%d req:%d min:%d\n",
        e->type, e->resourceid, e->serial, e->error_code,
        e->request_code, e->minor_code);
    return 0; // Ignore everything else.
}
Display * jbwm_open_display(void)
{
    Display * display;
    errno = ECONNREFUSED; // Default error message
    if (!(display = XOpenDisplay(NULL))) {
        enum { SZ = 128 };
        char buf[SZ];
        snprintf(buf, SZ, "Cannot open DISPLAY %s",
            getenv(JBWM_ENV_DISPLAY));
        jbwm_error(buf);
    }
    XSetErrorHandler(handle_xerror);
    return display;
}
