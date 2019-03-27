#include "testlib.h"
#include "malloc.h"

int main()
{
    test_start("malloc.c");
    initAllocator();

    void *a = my_malloc(16);
    my_malloc(3);
    my_malloc(28372);
    void *b = my_malloc(16);
    void *c = my_malloc(5);
    my_malloc(2112);

    dumpAllocator();

    my_free(b);
    my_free(c);
    my_free(a);
    dumpAllocator();
    my_malloc(16746540);
    my_malloc(1);
    my_malloc(1);
    my_malloc(1);
    my_malloc(1);
    dumpAllocator();

    return test_end();
}
