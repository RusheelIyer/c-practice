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
	test_assert(printed == (pass % 3 == 1), "File was correctly filtered");

	printed = 0;
	(void) dirp;
	e.d_name[0] = 'a' + pass;
	e.d_name[1] = 'b' + pass;
	e.d_name[2] = 'c' + pass;
	e.d_name[3] = 0;
	e.d_ino = 0;
	pass++;
	if (pass % 3 != 0) {
		e.d_name[3] = '.';
		e.d_name[4] = 'e';
		e.d_name[5] = 'a';
		e.d_name[6] = 0;
		if (pass % 3 == 1) {
			e.d_name[5] = 'x';
		}
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
	test_start("You filter files by extension.");
	test_plan(12);

	test_equals_int(list("dirname", "ex"), 0, "list succeeds");

	return test_end();
}

