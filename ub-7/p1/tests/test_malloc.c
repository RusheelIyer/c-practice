#include "testlib.h"
#include "barrier.h"
#include <stdlib.h>

static void *allocated;

void *__real_malloc(size_t size);
void *__wrap_malloc(size_t size) {
	allocated = __real_malloc(size);
	return allocated;
}

void free(void *ptr) {
	if (ptr == allocated)
		test_assert(1, "barrier is freed");
}

int main() {
	test_start("createBarrier allocates memory for the barrier using malloc, deleteBarrier frees the memory.");
	test_plan(2);

	ThreadBarrier *barrier = createBarrier(10);
	test_equals_ptr(barrier, allocated, "barrier memory is from malloc");
	deleteBarrier(barrier);

	return test_end();
}
