#include "testlib.h"
#include "malloc.h"

#define HEAP_SIZE (16 * 1024 * 1024)
extern uint8_t _heapData[HEAP_SIZE];

int main() {
    test_start("my_malloc returns the right addresses for multiple allocations");
    test_plan(4);
    initAllocator();

    void* alloc1 = my_malloc(5);
    test_equals_ptr(alloc1, _heapData + 16, "alloc1 is correct");

    void* alloc2 = my_malloc(5);
    test_equals_ptr(alloc2, _heapData + 32 + 16, "alloc2 is correct");

    void* alloc3 = my_malloc(128);
    test_equals_ptr(alloc3, _heapData + 32 + 32 + 16, "alloc3 is correct");

    void* alloc4 = my_malloc(1);
    test_equals_ptr(alloc4, _heapData + 32 + 32 + 32 + 128, "alloc4 is correct");

    return test_end();
}
