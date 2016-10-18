#include "file.h"
#include "util.h"
int main(int argc __attribute__((unused)),
char ** argv __attribute__((unused)))
{
	// test success
	fd_t f = jb_open("/dev/null", O_RDONLY);
	close(f);
	// test failure
	f = jb_open("/dev/does_not_exist", O_RDONLY);
	// test check
	jb_check(true, "true");
	jb_check(false, "false");
	return 0;
}
