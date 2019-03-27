#include "persistence.h"
#include <stdlib.h>
#include <string.h>

void make_persistent(struct tm **time)
{
    (void) time;
    struct tm *new;
    if ((time == NULL) || (*time == NULL))
        return;

    new = malloc(sizeof(struct tm));
    if (new == NULL)
        return;
    memcpy(new, *time, sizeof(struct tm));
    *time = new;
}

void free_persistent(struct tm **time)
{
    (void) time;
    if ((time == NULL) || (*time == NULL))
        return;

    free(*time);
    *time = NULL;
}
