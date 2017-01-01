// Copyright 2017, Jeffrey E. Bedard
#include "file.h"
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
fd_t jb_open(const char * path, const int flags)
{
	fd_t f = open(path, flags);
	if (f == -1) {
		char msg[] = "Cannot open file ";
		write(2, msg, sizeof(msg));
		uint16_t l = 0;
		while(path[++l]);
		write(2, path, l);
		write(2, "\n", 1);
	}
	return f;
}
