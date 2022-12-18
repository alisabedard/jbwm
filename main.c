// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Alisa Bedard <alisabedard@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#include <string.h>
#include "JBWMScreen.h"
#include "atom.h"
#include "command_line.h"
#include "display.h"
#include "events.h"
#include "font.h"
#include "jbwm.h"
int main(int argc, char **argv)
{
  Display * d;
  jbwm_parse_command_line(argc, argv);
  d = jbwm_open_display();
  jbwm_create_atoms(d);
  jbwm_open_font(d);
  {
    struct JBWMClient *current_client, *head_client;
    uint8_t const n = ScreenCount(d);
    // allocate using dynamically sized array on stack
    struct JBWMScreen s[n]; // remains in scope till exit.
    memset(s,0,sizeof(s));
    current_client=NULL; // no clients added yet
    head_client=NULL; // flag as empty list
    jbwm_set_up_error_handler(&head_client, &current_client);
    jbwm_init_screens(d, s, &head_client, &current_client,
      n - 1); // -1 for index
    jbwm_events_loop(s,&head_client,&current_client); // does not return
  }
}
