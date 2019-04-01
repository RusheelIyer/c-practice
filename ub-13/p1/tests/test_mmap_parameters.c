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
	(void) pathname;
	return 5;
}

//void *mem = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
	(void) addr;
	(void) length;
	(void) prot;
	(void) flags;
	(void) offset;
	(void) fd;
	test_equals_int64((int64_t) length, 580, "mmap: length is correct");
	test_equals_ptr(addr, NULL, "mmap: addr is correct");
	test_assert(prot & PROT_READ, "mmap: PROT_READ is set");
	//test_assert(prot & MAP_PRIVATE, "mmap: MAP_PRIVATE is set");
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
	test_start("You pass the right parameters to mmap.");
	test_plan(4);

	FileSystem *fs = mapFileSystem("file");
	test_equals_ptr(fs, (void*) 1024, "You return the pointer from mmap");

	return test_end();
}

