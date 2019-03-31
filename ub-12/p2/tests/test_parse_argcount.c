#include "testlib.h"
#include "copy.h"
#include <getopt.h>

int main() {
	test_start("Your parseCopyArgs returns -1 if the wrong number of arguments is given.");

	CopyArgs args;

	optind = 0;
	char *argv2[] = { "copy", "-s", "2", "from"};
	test_equals_int(parseCopyArgs(4, argv2, &args), -1, "copy -s 2 from: parse fails");

	optind = 0;
	char *argv3[] = { "copy", "un", "from", "to" };
	test_equals_int(parseCopyArgs(4, argv3, &args), -1, "copy un from to: parse fails");

	optind = 0;
	char *argv4[] = { "copy", "-c", "4", "from", "to", "x", "y" };
	test_equals_int(parseCopyArgs(7, argv4, &args), -1, "copy -c 4 from to x y: parse fails");


	return test_end();
}

