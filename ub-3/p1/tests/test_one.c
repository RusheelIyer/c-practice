#include "testlib.h"
#include "malloc.h"

#define HEAP_SIZE (16 * 1024 * 1024)
extern uint8_t _heapData[HEAP_SIZE];

int main() {
    test_start("my_malloc returns the first block of memory on the first allocation.");
	test_plan(1);
    initAllocator();

    void* alloc1 = my_malloc(5);
    test_equals_ptr(alloc1, _heapData + 16, "alloc1 is the first block of memory");

    return test_end();
}
