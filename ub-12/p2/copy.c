#define _POSIX_C_SOURCE 2
#include "copy.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

// Intermediate copy buffer. Use this buffer for your read() and write() calls.
char _buffer[BUFFER_SIZE];

int parseCopyArgs(int argc, char * const argv[], CopyArgs* args)
{
    if (args == NULL) {
        return -1;
    }

    // Initialize with default values
    args->skip  = 0;
    args->count = -1;

    (void) argc;
    (void) argv;

    // ----------------
    // Parse the arguments.
    // ----------------

    return -1;
}

int doCopy(CopyArgs* args)
{
    if (args == NULL) {
        return -1;
    }

    // ----------------
    // Copy the file.
    // ----------------

    return -1;
}
