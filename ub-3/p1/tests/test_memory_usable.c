#include "testlib.h"
#include "malloc.h"
#include <stdio.h>

static void fillMemory(uint8_t *mem, uint64_t size) {
	while (size--)
		*(mem++) = (uint8_t) size;
}

static void verifyMemory(uint8_t *mem, uint64_t size) {
	static char msg[50];
	while (size--) {
		sprintf(msg, "Memory at %p is usable", mem);
		test_equals_int(*(mem++), (uint8_t) size, msg);
	}
}

int main() {
	test_start("Memory returned by my_malloc is usable");
	initAllocator();

	uint8_t *alloc1 = my_malloc(32);
	fillMemory(alloc1, 32);
	uint8_t *alloc2 = my_malloc(16);
	fillMemory(alloc2, 16);
	verifyMemory(alloc1, 32);
	my_free(alloc1);
	alloc1 = my_malloc(32);
	fillMemory(alloc1, 32);
	verifyMemory(alloc2, 16);
	my_free(alloc2);
	verifyMemory(alloc1, 32);

	return test_end();
}
