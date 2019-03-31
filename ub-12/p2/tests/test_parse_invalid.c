#include "testlib.h"
#include "copy.h"
#include <getopt.h>

int main() {
	test_start("Your parseCopyArgs returns -1 for invalid flags.");

	CopyArgs args;

	optind = 0;
	char *argv2[] = { "copy", "-x", "2", "from", "to" };
	test_equals_int(parseCopyArgs(5, argv2, &args), -1, "copy -x 2 from to: parse fails");

	return test_end();
}

