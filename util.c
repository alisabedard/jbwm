// Copyright 2016, Jeffrey E. Bedard

#include "util.h"

#include "log.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

void jb_close(const fd_t fd)
{
	if (close(fd) != -1)
		return;
	const uint8_t sz = 32;
	char buf[sz];
	snprintf(buf, sz, "Could not close fd %d", fd);
	perror(buf);
}

bool jb_check(const bool val, const char * msg)
{
	if (!val) {
		WARN("%s", msg);
	}
	return !val;
}

