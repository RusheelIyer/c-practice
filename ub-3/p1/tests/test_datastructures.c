#include "testlib.h"
#include "malloc.h"

#define HEAP_SIZE (16 * 1024 * 1024)
extern void *_firstFreeBlock;

int main() {
    test_start("After malloc, your datastructures match the expected values.");
    test_plan(6);
    initAllocator();

    void* alloc1 = my_malloc(5);

    void* alloc2 = my_malloc(5);

    void* alloc3 = my_malloc(128);

    test_equals_int64(*((uint64_t*)(alloc1 - 8)), 32, "size of alloc1 is 32 bytes");
    test_equals_int64(*((uint64_t*)(alloc2 - 8)), 32, "size of alloc2 is 32 bytes");
    test_equals_int64(*((uint64_t*)(alloc3 - 8)), 128 + 16, "size of alloc3 is 128+16 bytes");
    test_equals_ptr(*((void**) (alloc3 + 128)), (void*) 0, "The block after alloc3 is the last block");
    test_equals_int64(*((uint64_t*)(alloc3 + 128 + 8)), HEAP_SIZE - 32 - 32 - 128 - 16, "size of the last block");
    test_equals_ptr(_firstFreeBlock, alloc3 + 128, "The last block is the first free block");

    return test_end();
}
