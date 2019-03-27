#include "testlib.h"
#include "malloc.h"

#define HEAP_SIZE (16 * 1024 * 1024)
extern uint8_t _heapData[HEAP_SIZE];

int main() {
    test_start("my_malloc returns NULL when the memory does not have a long enough free space.");
    test_plan(5);
    initAllocator();

    void* alloc1 = my_malloc(HEAP_SIZE - 128);
    test_equals_ptr(alloc1, _heapData + 16, "alloc1 is correct");

    void* alloc2 = my_malloc(300);
    test_equals_ptr(alloc2, (void*) 0, "alloc2 failed");

    my_free(alloc1);

    void* alloc3 = my_malloc(1);
    test_equals_ptr(alloc3, _heapData + 16, "alloc3 is correct");

    void* alloc4 = my_malloc(HEAP_SIZE - 16 - 32);
    test_equals_ptr(alloc4, _heapData + 16 + 32, "alloc4 is correct");

    void* alloc5 = my_malloc(1);
    test_equals_ptr(alloc5, (void*) 0, "alloc5 failed");

    return test_end();
}
