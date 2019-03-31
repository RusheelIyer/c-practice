#include "testlib.h"
#include "copy.h"
#include <getopt.h>

int main() {
	test_start("You parse the skip argument if it is given and return the default otherwise.");

	char *argv[] = { "copy", "from", "to" };
	CopyArgs args;
	args.skip = 42;
	test_equals_int(parseCopyArgs(3, argv, &args), 0, "copy from to: parse succeeds");
	test_equals_int(args.skip, 0, "copy from to: skip has default value");

	optind = 0;
	char *argv2[] = { "copy", "-s", "2", "from", "to" };
	test_equals_int(parseCopyArgs(5, argv2, &args), 0, "copy -s 2 from to: parse succeeds");
	test_equals_int(args.skip, 2, "copy -s 2 from to: skip has correct value");

	optind = 0;
	char *argv3[] = { "copy", "-s", "5", "-c", "3", "from", "to" };
	test_equals_int(parseCopyArgs(7, argv3, &args), 0, "copy -s 5 -c 3 from to: parse succeeds");
	test_equals_int(args.skip, 5, "copy -s 5 -c 3 from to: skip has correct value");

	return test_end();
}

