#include "testlib.h"
#include "ls.h"
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int dir;
struct dirent e;
int pass;
int printed = 0;

DIR *opendir(const char *name) {
	(void) name;
	return (DIR*) &dir;
}

struct dirent *readdir(DIR *dirp) {
	test_assert(printed == (pass % 2 != 0), "File was correctly skipped or printed");

	printed = 0;
	(void) dirp;
	e.d_name[0] = 'a' + pass;
	e.d_name[1] = 'b' + pass;
	e.d_name[2] = 'c' + pass;
	e.d_name[3] = 0;
	e.d_ino = 0;
	pass++;
	if (pass % 2 == 0) {
		e.d_name[0] = '.';
	}
	return pass > 10 ? NULL : &e;
}

int closedir(DIR *dirp) {
	(void) dirp;
	return 0;
}

int __xstat(int __ver, const char *__filename, struct stat *__stat_buf) {
	(void) __ver;
	(void) __filename;
	__stat_buf->st_blocks = 3;
	__stat_buf->st_size = 123;
	return 0;
}

void _printLine(unsigned int size, unsigned int sizeOnDisk, const char* name) {
	(void) name;
	(void) size;
	(void) sizeOnDisk;
	printed = 1;
}

int main() {
	test_start("You skip files starting with '.'");
	test_plan(12);

	test_equals_int(list("dirname", NULL), 0, "list succeeds");

	return test_end();
}

