#include "print.h"
#include <stdio.h>

void print_line(int64_t number, char *string)
{
    (void) number;
    (void) string;

    printf("%ld - %s\n", number, string);

}
