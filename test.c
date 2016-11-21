#define DEBUG
#include "file.h"
#include "log.h"
#include "util.h"
int main(int argc __attribute__((unused)),
char ** argv __attribute__((unused)))
{
	// test success
	fd_t f = jb_open("/dev/null", O_RDONLY);
	close(f);
	// test failure
#define TF "/dev/does_not_exist"
	f = jb_open(TF, O_RDONLY);
	jb_check(f != -1, TF);
	// test check
	jb_check(true, "true");
	jb_check(false, "false");
	// test log
	LOG("LOG %s", "param");
	return 0;
}
