// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "command_line.h"
#include "display.h"
#include "events.h"
#include "font.h"
#include "JBWMClient.h"
#include "jbwm.h"
#include <string.h>
#include "vdesk.h"
int main(int argc, char **argv)
{
    Display * d;
    jbwm_parse_command_line(argc, argv);
    d = jbwm_open_display();
    jbwm_open_font(d);
    {
        uint8_t const n = ScreenCount(d);
        // allocate using dynamically sized array on stack
        struct JBWMScreen s[n]; // remains in scope till exit.
        memset(s,0x00,sizeof(s));
        jbwm_init_screens(d, s, n - 1); // -1 for index
        jbwm_events_loop(s); // does not return
    }
}
