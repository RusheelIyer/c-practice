#include "testlib.h"
#include "print.h"

int main()
{
    test_start("print.c");

    // Add some more test code here.
    print_line(42, "Hello World!");

    return test_end();
}
