// Copyright 2020, Jeffrey E. Bedard
#include "display.h"
#include <X11/Xproto.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "config.h"
#include "log.h"
#include "util.h"
static struct JBWMClient ** head;
void jbwm_set_up_error_handler(struct JBWMClient ** head_client){
    head=head_client;
}
__attribute__((pure))
static int handle_xerror(Display * d __attribute__((unused)),
    XErrorEvent * restrict e)
{
    struct JBWMClient *c;
    switch (e->error_code) {
    case BadAccess:
        if (e->request_code == X_ChangeWindowAttributes)
            jbwm_error("Cannot access the root window.");
        break;
    case BadWindow:
        if(head){
            c=jbwm_find_client(*head, e->resourceid);
            if(c)
                jbwm_client_free(c, head);
        }
        break;
    case BadAtom:
        break;
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
    display=XOpenDisplay(NULL);
    if(!display)
        jbwm_error(getenv("DISPLAY"));
    XSetErrorHandler(handle_xerror);
    return display;
}
