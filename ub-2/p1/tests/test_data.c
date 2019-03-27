#include "testlib.h"
#include "persistence.h"
#include <stdio.h>
#include <string.h>

int main()
{
    test_start("You changed the pointer and copied all data.");
    test_plan(3);

    time_t timecode1 = 1388530800;
    time_t timecode2 = 1420066800;

    struct tm *time1 = gmtime(&timecode1);
    struct tm *time1_prev = time1;
    make_persistent(&time1);

    test_assert(time1 != time1_prev, "The pointer is different");
    test_assert(strcmp(asctime(time1), asctime(time1_prev)) == 0, "The new struct tm contains the same data");

    struct tm *time2 = gmtime(&timecode2);

    test_assert(time1->tm_year != time2->tm_year, "A second call to gmtime() does not overwrite the persistent copy");

    free_persistent(&time1);
    return test_end();
}
