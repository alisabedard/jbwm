#ifndef MISC_H
#define MISC_H

__attribute__ ((noreturn))
void
jbwm_exit(const int status);

int
handle_xerror(Display * dpy, XErrorEvent * e);

#endif
