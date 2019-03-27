#include "testlib.h"
#include "malloc.h"

int main() {
    test_start("Free memory regions are merged.");
    test_plan(1);
    initAllocator();

    my_malloc(32);
    void* alloc1 = my_malloc(16);
    void* alloc2 = my_malloc(16);
    my_malloc(32);
    my_free(alloc1);
    my_free(alloc2);

    void* alloc3 = my_malloc(48);
    test_equals_ptr(alloc3, alloc1, "The new allocation is the same as the freed regions");

    return test_end();
}
