#include "testlib.h"
#include "malloc.h"

extern void *_firstFreeBlock;

int main() {
    test_start("free updates the pointer to the first block.");
    test_plan(1);
    initAllocator();

    my_malloc(32);
    void *oldFreePointer = _firstFreeBlock;
    void* alloc1 = my_malloc(32);
    my_malloc(32);
    my_free(alloc1);

    test_equals_ptr(_firstFreeBlock, oldFreePointer, "_firstFreeBlock is correct");

    return test_end();
}
