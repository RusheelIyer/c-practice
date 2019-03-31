#include "testlib.h"
#include "copy.h"
#include <getopt.h>

int main() {
	test_start("You parse the count argument if it is given and return the default otherwise.");

	char *argv[] = { "copy", "from", "to" };
	CopyArgs args;
	args.count = 42;
	test_equals_int(parseCopyArgs(3, argv, &args), 0, "copy from to: parse succeeds");
	test_equals_int(args.count, -1, "copy from to: count has default value");

	optind = 0;
	char *argv2[] = { "copy", "-c", "2", "from", "to" };
	test_equals_int(parseCopyArgs(5, argv2, &args), 0, "copy -c 2 from to: parse succeeds");
	test_equals_int(args.count, 2, "copy -c 2 from to: count has correct value");

	optind = 0;
	char *argv3[] = { "copy", "-s", "5", "-c", "3", "from", "to" };
	test_equals_int(parseCopyArgs(7, argv3, &args), 0, "copy -s 5 -c 3 from to: parse succeeds");
	test_equals_int(args.count, 3, "copy -s 5 -c 3 from to: count has correct value");

	return test_end();
}

