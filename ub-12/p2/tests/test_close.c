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

int open_from = 2738, open_to = 27328;
int __real_open(const char *pathname, int flags, ...);
int __wrap_open(const char *pathname, int flags, ...) {
	int res = __real_open(pathname, flags, 0666);

	if (strcmp(pathname, "from") == 0) {
		open_from = res;
	}
	if (strcmp(pathname, "to") == 0) {
		open_to = res;
	}
	return res;
}

int close(int fd) {
	if (fd == open_from) {
		test_assert(1, "You close from");
	}
	if (fd == open_to) {
		test_assert(1, "You close to");
	}
	return 0;
}

const char* teststr = "This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.";

int main() {
	test_start("You close both files.");
	test_plan(2);

	remove("from");
	remove("to");

	// Create from file.
	int fd = open("from", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
	if (fd == -1)
		test_failed_message("open");
	if (write(fd, teststr, strlen(teststr)) < 0)
		test_failed_message("write");
	open_from = -2;
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

