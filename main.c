// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "JBWMScreen.h"
#include "events.h"
#include "display.h"
#include "font.h"
#include "jbwm.h"
#include "screen.h"
int main(int argc, char **argv)
{
	jbwm_set_defaults();
	jbwm_parse_command_line(argc, argv);
	Display * d = jbwm_open_display();
	jbwm_open_font(d);
	uint8_t i = ScreenCount(d);
	// allocate using dynamically sized array on stack
	struct JBWMScreen s[i]; // remains in scope till exit.
	jbwm_set_screens(s);
	while (i--)
		jbwm_init_screen(d, i);
	jbwm_events_loop(d);
	return 0;
}
