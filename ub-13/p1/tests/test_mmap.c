#include "testlib.h"
#include "filesystem.h"
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int close(int fd) {
	(void) fd;
	return 0;
}

int open(const char *pathname, int flags, ...) {
	(void) flags;
    test_equals_string(pathname, "file", "You open the correct file");
	return 5;
}

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
	(void) addr;
	(void) length;
	(void) prot;
	(void) flags;
	(void) offset;
    test_equals_int(fd, 5, "You pass the correct fd to mmap");
	return (void*) 1024;
}
int munmap(void *addr, size_t length) {
	(void) addr;
	(void) length;
	return 0;
}

int __fxstat(int __ver, const int fd, struct stat *__stat_buf) {
	(void) __ver;
	(void) fd;
	__stat_buf->st_blocks = 2;
	__stat_buf->st_size = 580;
	return 0;
}

int __xstat(int __ver, const char *__filename, struct stat *__stat_buf) {
	(void) __ver;
	(void) __filename;
	__stat_buf->st_blocks = 2;
	__stat_buf->st_size = 580;
	return 0;
}

int main() {
	test_start("You open the file and pass the file handle to mmap.");
    test_plan(3);

	FileSystem *fs = mapFileSystem("file");
	test_equals_ptr(fs, (void*) 1024, "You return the pointer from mmap");

	return test_end();
}

