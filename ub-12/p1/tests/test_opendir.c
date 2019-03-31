#include "testlib.h"
#include "ls.h"
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int dir;

DIR *opendir(const char *name) {
	(void) name;
    test_equals_string(name, "dirname", "You call opendir with the correct directory");
	return (DIR*) &dir;
}

struct dirent *readdir(DIR *dirp) {
	(void) dirp;
	return NULL;
}

int closedir(DIR *dirp) {
	(void) dirp;
	return 0;
}

int stat(const char *path, struct stat *buf) {
	(void) path;
	buf->st_blocks = 0;
	buf->st_size = 0;
	return 0;
}

int main() {
	test_start("Your list calls opendir do open the directory.");
    test_plan(2);

	test_equals_int(list("dirname", NULL), 0, "list succeeds");

	return test_end();
}

