#define _POSIX_C_SOURCE 2
#include "testlib.h"
#include "copy.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

int __real_open(const char *pathname, int flags, ...);
int __wrap_open(const char *pathname, int flags, ...) {
	if (strcmp(pathname, "to") == 0) {
		test_assert(1, "You call open with the correct file");
		test_assert(flags & O_CREAT, "You set the O_CREAT flag");
		test_assert(flags & O_EXCL, "You set the O_EXCL flag");
	}
	return (__real_open(pathname, flags, 0666));
}

const char* teststr = "This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.";

int main() {
	test_start("You open the file you need to write to (using the right flags).");
	test_plan(3);

	remove("from");
	remove("to");

	// Create from file.
	int fd = open("from", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
	if (fd == -1)
		test_failed_message("open");
	if (write(fd, teststr, strlen(teststr)) < 0)
		test_failed_message("write");
	close(fd);

	CopyArgs args = {
		.count = -1,
		.skip = 0,
		.from = "from",
		.to = "to"
	};
	doCopy(&args);

	return test_end();
}

