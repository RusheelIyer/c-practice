#include "testlib.h"
#include "persistence.h"
#include <stdio.h>

int main()
{
    test_start("persistence.c");
    time_t timecode1 = 1388530800;
    time_t timecode2 = 1420066800;

    struct tm *time1 = gmtime(&timecode1);
    make_persistent(&time1);
    struct tm *time2 = gmtime(&timecode2);

    printf("Time 1: %s", asctime(time1));
    printf("Time 2: %s", asctime(time2));

    free_persistent(&time1);
    return 0;
}