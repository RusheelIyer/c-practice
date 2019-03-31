#include "testlib.h"
#include "ls.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
    test_start("ls.c");

    printf("All temporary files:\n");
    printf("   size    |  on disk   |  name\n");
    test_equals_int(list("/tmp", NULL), 0, "list /tmp succeeds");

    printf("\n");
    printf("All c files in this directory:\n");
    printf("   size    |  on disk   |  name\n");
    test_equals_int(list(".", "c"), 0, "list c files succeeds");

    return test_end();
}
