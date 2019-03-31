#define _POSIX_C_SOURCE 2
#include "copy.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

// Intermediate copy buffer. Use this buffer for your read() and write() calls.
char _buffer[BUFFER_SIZE];

int parseCopyArgs(int argc, char* const argv[], CopyArgs* args)
{
    if (args == NULL) {
        return -1;
    }

    // Initialize with default values
    args->skip = 0;
    args->count = -1;

    const char* optstring = "s: c:";
    int arguments = getopt(argc, argv, optstring);

    while (arguments != -1) {
        switch (arguments) {
        case 's':
            args->skip = atoi(optarg);
            break;
        case 'c':
            args->count = atoi(optarg);
            break;
        default:
            return -1;
        }
        arguments = getopt(argc, argv, optstring);
    }

    if (optind != argc - 2) {
        return -1; // We expect two parameters after the options.
    }

    args->from = argv[optind];
    args->to   = argv[optind + 1];

    return 0;
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
