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

DIR *opendir(const char *name) {
    (void) name;
    return (DIR*) &dir;
}

struct dirent *readdir(DIR *dirp) {
    (void) dirp;
    e.d_name[0] = 'a' + pass;
    e.d_name[1] = 0;
    e.d_ino = 0;
    pass++;
    return pass > 3 ? NULL : &e;
}

int closedir(DIR *dirp) {
    test_equals_ptr(dirp, (DIR*) &dir, "You call closedir with the correct pointer");
    return 0;
}

int __xstat (int __ver, const char *__filename,
                    struct stat *__stat_buf) {
    (void) __ver;
    (void) __filename;
    __stat_buf->st_blocks = 3;
    __stat_buf->st_size = 123;
    return 0;
}

int main() {
	test_start("Your list calls closedir.");
	test_plan(2);

	test_equals_int(list("dirname", NULL), 0, "list succeeds");

	return test_end();
}

