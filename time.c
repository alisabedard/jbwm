// Copyright 2017, Jeffrey E. Bedard
#include "time.h"
#include <time.h>
void jb_sleep(uint32_t miliseconds)
{
	struct timespec t = {.tv_nsec = miliseconds * 1000000};
	while (nanosleep(&t, &t))
		;
}
