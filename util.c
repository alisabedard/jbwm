// Copyright 2016, Jeffrey E. Bedard

#include "util.h"

#include "log.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
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

fd_t jb_open(const char * path, const int flags)
{
	fd_t r = open(path, flags);
	const uint8_t sz = 64;
	char buf[sz];
	snprintf(buf, sz, "Could not open %s", path);
	jb_check(r != -1, buf);
	return r;
}

bool jb_check(const bool val, const char * msg)
{
	if (!val) {
		if (errno)
			perror(msg ? msg : "Error");
		else
			fprintf(stderr, "%s\n", msg ? msg
				: "Warning:  Value check failed");
	}
	errno = 0; // prevent stale errno values from persisting.
	return !val;
}

void jb_assert(const bool val, const char * msg)
{
	if (jb_check(val, msg))
		exit(1);
}

