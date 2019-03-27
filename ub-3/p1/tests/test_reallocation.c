#include "testlib.h"
#include "malloc.h"

int main() {
    test_start("After a my_free(), a my_malloc re-allocates that memory.");
    test_plan(2);
    initAllocator();

    my_malloc(32);
    void* alloc1 = my_malloc(64);
    my_malloc(32);
    my_free(alloc1);

    void* alloc2 = my_malloc(16);
    test_equals_ptr(alloc2, alloc1, "The new allocation is the same as the old (different size)");

    void* alloc3 = my_malloc(32);
    my_malloc(32);
    my_free(alloc3);

    void* alloc4 = my_malloc(32);
    test_equals_ptr(alloc3, alloc4, "The new allocation is the same as the old (same size)");

    return test_end();
}
