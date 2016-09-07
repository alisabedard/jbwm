#include <stdio.h>

#define DEBUG

#include "util.h"

int main(int argc, char ** argv)
{
	puts("Testing libjb..");
	jb_check(true, "true");
	int a = 0;
	int b = 1;
	jb_check(a == b, "a != b");
	jb_check(false, "false");
}

