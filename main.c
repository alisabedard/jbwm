// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "command_line.h"
#include "display.h"
#include "events.h"
#include "font.h"
#include "jbwm.h"
#include "screen.h"
int main(int argc, char **argv)
{
    jbwm_set_defaults();
    jbwm_parse_command_line(argc, argv);
    Display * d = jbwm_open_display();
    jbwm_open_font(d);
    // allocate using dynamically sized array on stack
    struct JBWMScreen s[ScreenCount(d)]; // remains in scope till exit.
    jbwm_set_screens(s);
    jbwm_init_screens(d, ScreenCount(d) - 1); // -1 for index
    jbwm_events_loop(d); // does not return
}
