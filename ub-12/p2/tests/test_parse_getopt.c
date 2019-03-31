#include "testlib.h"
#include "copy.h"
#include <unistd.h>
#include <getopt.h>

int __posix_getopt(int argc, char * const argv[], const char *optstring) {
	(void) argc;
	(void) argv;
	(void) optstring;
	test_assert(1, "You use __posix_getopt");
	return -1;
}

int getopt(int argc, char * const argv[], const char *optstring) {
	(void) argc;
	(void) argv;
	(void) optstring;
	test_assert(1, "You use getopt");
	return -1;
}

int main() {
	test_start("You use getopt to parse the arguments.");
	test_plan(1);

	char *argv[] = { "copy", "from", "to" };
	CopyArgs args;
	parseCopyArgs(3, argv, &args);

	return test_end();
}

