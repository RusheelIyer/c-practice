#include "testlib.h"
#include "copy.h"

int main() {
	test_start("You parse the last two arguments as source and destination.");

	char *argv[] = { "copy", "from", "to" };
	CopyArgs args;
	parseCopyArgs(3, argv, &args);

	test_equals_string(args.from, "from", "copy from to: source is correct");
	test_equals_string(args.to, "to", "copy from to: destination is correct");


	char *argv2[] = { "copy", "-s", "0", "from", "to" };
	parseCopyArgs(5, argv2, &args);

	test_equals_string(args.from, "from", "copy -s 0 from to: source is correct");
	test_equals_string(args.to, "to", "copy -s 0 from to: destination is correct");

	return test_end();
}

