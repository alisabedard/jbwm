// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "atom.h"
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
    jbwm_create_atoms(d);
    jbwm_open_font(d);
    {
        struct JBWMClient *current_client;
        uint8_t const n = ScreenCount(d);
        // allocate using dynamically sized array on stack
        struct JBWMScreen s[n]; // remains in scope till exit.
        memset(s,0,sizeof(s));
        current_client=NULL; // flag as empty list
        jbwm_init_screens(d, s, &current_client, n - 1); // -1 for index
        jbwm_events_loop(s,&current_client); // does not return
    }
}
