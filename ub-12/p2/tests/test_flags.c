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

const char* teststr = "This is a test string.This is a test string.This is a test string."
		"This is a test string.This is a test string.This is a test string."
		"This is a test string.This is a test string.This is a test string."
		"This is a test string.This is a test string.This is a test string."
		"This is a test string.This is a test string.This is a test string."
		"This is a test string.This is a test string.";
char testbuff[10000];

#define S 10
#define C 40

int main() {
	test_start("Your copy skips and accepts the length parameter.");

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
		.count = C,
		.skip = S,
		.from = "from",
		.to = "to"
	};
	doCopy(&args);

	int fd2 = open("to", O_RDONLY);
	if (fd2 == -1) {
		test_failed_message("open(to)");
	}
	if (read(fd2, testbuff, C) < 0)
		test_failed_message("read(to)");
	close(fd2);
	testbuff[C] = 0;
	int success = 1;
	for (int i = 0; i < C; i++) {
		if (testbuff[i] != teststr[i + S]) {
			success = 0;
			break;
		}
	}
	test_assert(success, "to file contents are correct");

	// size:
	struct stat buf;
	buf.st_size = 0;
	stat("to", &buf);
	test_equals_int(buf.st_size, C, "to file size is correct");

	return test_end();
}

