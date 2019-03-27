#ifndef PERSISTENCE_H
#define PERSISTENCE_H
#include <time.h>

void make_persistent(struct tm **time);

void free_persistent(struct tm **time);

#endif