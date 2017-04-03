// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#include "signal.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "client.h"
#include "display.h"
#include "JBWMClient.h"
#include "log.h"
#include "title_bar.h"
static void atexit_cb(void)
{
	JBWM_LOG("atexit_cb");
	struct JBWMClient * i = jbwm_get_head_client();
	while (i) {
#ifdef JBWM_USE_TITLE_BAR
		// Some clients crash if left shaded at exit:
		if (i->opt.shaded) {
			jbwm_toggle_shade(i);
		}
#endif//JBWM_USE_TITLE_BAR
		// swap:
		struct JBWMClient * j = i->next;
		free(i);
		i = j;
	}
	/*  exit() closes the display when it closes all open file
	 *  descriptors.  Trying to do so manually results in exit hanging on
	 *  the first invocation and a second invocation being necessary.  */
}
static void signal_cb(int sig)
{
	JBWM_LOG("signal_cb: %d", sig);
	exit(sig); // Pass the signal to the exit handler
}
static void sigchld_cb(int sig __attribute__((unused)))
{
	wait(NULL);
}
void jbwm_wait_child(void)
{
	signal(SIGCHLD, sigchld_cb); // reap zombies
}
void jbwm_set_signal_handler(void)
{
	atexit(atexit_cb);
	signal(SIGINT, signal_cb); // catch break
}
