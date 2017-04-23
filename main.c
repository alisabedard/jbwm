#include "jbwm.h"
#include "JBWMScreen.h"
#include "display.h"
#include "events.h"
#include "signal.h"
int main(int argc, char **argv)
{
	jbwm_set_defaults();
	jbwm_parse_command_line(argc, argv);
	Display * d = jbwm_open_display();
	uint8_t i = ScreenCount(d);
	// allocate using dynamically sized array on stack
	struct JBWMScreen s[i]; // remains in scope till exit.
	jbwm_set_screens(s);
	while (i--)
		jbwm_init_screen(d, i);
	jbwm_set_signal_handler();
	jbwm_events_loop(d);
	return 0;
}
