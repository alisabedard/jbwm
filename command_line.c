// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "command_line.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "key_masks.h"
#include "log.h"
/* Used for overriding the default WM modifiers.  Note on type: key masks are
 * 16 bits maximum, therefore we return a uint16_t. */
__attribute__((warn_unused_result))
static uint16_t parse_modifiers(char * arg)
{
	JBWM_LOG("parse_modifiers()");
	const size_t s = strlen(arg);
	if (s > 3) { // switch based on the 4th character
		switch(arg[3]) {
		case 'f': // shift
			return ShiftMask;
		case 'k': // lock
			return LockMask;
		case 't': // control
		case 'l': // ctrl
			return ControlMask;
		case '2': // mod2
			return Mod2Mask;
		case '3': // mod3
			return Mod3Mask;
		case '4': // mod4
			return Mod4Mask;
		case '5': // mod5
			return Mod5Mask;
		}
	}
	// everything else becomes mod1
	return Mod1Mask;
}
void jbwm_parse_command_line(const int argc, char **argv)
{
	JBWM_LOG("parse_argv(%d,%s...)", argc, argv[0]);
	static const char optstring[] = "1:2:b:d:F:f:hs:Vv";
	int8_t opt;
#define OVERRIDE(x) setenv(JBWM_ENV_##x, optarg, 1);
	while((opt = getopt(argc, argv, optstring)) != -1)
		switch (opt) {
		case '1':
			jbwm_set_grab_mask(parse_modifiers(optarg));
			break;
		case '2':
			jbwm_set_mod_mask(parse_modifiers(optarg));
			break;
		case 'b':
			OVERRIDE(BG);
			break;
		case 'd':
			OVERRIDE(DISPLAY);
			break;
		case 'F':
			OVERRIDE(FONT);
			break;
		case 'f':
			OVERRIDE(FG);
			break;
		case 's':
			OVERRIDE(FC);
			break;
		case 'V':
		case 'v':
			printf("%s version %s\n", argv[0], VERSION);
			exit(0);
		default:
			printf("%s -[%s]\n", argv[0], optstring);
			exit(1);
		}
}

