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

int check_open = 0;
int __real_open(const char *pathname, int flags, ...);
int __wrap_open(const char *pathname, int flags, ...) {
	if (check_open && strcmp(pathname, "from") == 0) {
		test_assert(1, "You call open with the correct file");
	}
	return (__real_open(pathname, flags, 0666));
}

const char* teststr = "This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.This is a test string.";

int main() {
	test_start("You open the file you need to read.");
	test_plan(1);

	remove("from");
	remove("to");

	// Create from file.
	int fd = open("from", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
	if (fd == -1)
		test_failed_message("open");
	if (write(fd, teststr, strlen(teststr)) < 0)
		test_failed_message("write");
	close(fd);

	check_open = 1;
	CopyArgs args = {
		.count = -1,
		.skip = 0,
		.from = "from",
		.to = "to"
	};
	doCopy(&args);

	return test_end();
}

