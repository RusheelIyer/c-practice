#define _POSIX_C_SOURCE 2
#include "copy.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

// Intermediate copy buffer. Use this buffer for your read() and write() calls.
static char _buffer[BUFFER_SIZE];

int parseCopyArgs(int argc, char * const argv[], CopyArgs* args)
{
    if (args == NULL) {
        return -1;
    }

    // Initialize with default values
    args->skip  = 0;
    args->count = -1;

    int opt;
    while ((opt = getopt(argc, argv, "s:c:")) != -1) {
        switch (opt) {
            case 's':
                args->skip = atoi(optarg);

                if ((errno != 0) || (args->skip < 0)) {
                    return -1; // Invalid skip argument.
                }

                break;

            case 'c':
                args->count = atoi(optarg);

                if ((errno != 0) || (args->count < -1)) {
                    return -1; // Invalid count argument
                }

                break;

            default: /* '?' */
                return -1;
        }
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
    int res = -1;

    if (args == NULL) {
        return -1;
    }

    // Open the source file in read-only mode.
    int fdFrom = open(args->from, O_RDONLY);
    if (fdFrom == -1) {
        fprintf(stderr, "Error: Failed to open source %s.\n", args->from);
        return -1;
    }

    // Get the length of the input file and move the file pointer to the offset
    // we start copying. Then compute the position up to which we need to copy.
    off_t fromSize = lseek(fdFrom, 0, SEEK_END);
    if (fromSize == -1) {
        fprintf(stderr, "Error: Failed to get file size of %s.\n", args->from);
        goto out_from;
    }

    if (lseek(fdFrom, args->skip, SEEK_SET) == -1) {
        fprintf(stderr, "Error: Failed to seek to start position %s.\n",
                args->from);
        goto out_from;
    }

    off_t pos = args->skip;
    off_t end;
    if (args->count >= 0) {
        end = pos + args->count;
        if (end > fromSize) {
            fprintf(stderr, "Error: Attempted to read %d bytes from offset "
                    "%ld, but file only has %ld bytes.\n",
                    args->count, pos, fromSize);
            goto out_from;
        }
    } else {
        end = fromSize;
    }

    // Create the destination file.
    int fdTo = open(args->to, O_WRONLY | O_CREAT | O_EXCL,
                    S_IRUSR | S_IWUSR | S_IRGRP);
    if (fdTo == -1) {
        fprintf(stderr, "Error: Failed to create destination %s.\n", args->to);
        goto out_from;
    }

    // Perform the copy operation.
    while (pos < end) {
        size_t left   = end - pos;
        size_t toRead = (left > BUFFER_SIZE) ? BUFFER_SIZE : left;

        ssize_t n = read(fdFrom, _buffer, toRead);
        if (n <= 0) {
            fprintf(stderr, "Error: Failed to read %zd bytes at offset %ld.\n",
                    n, pos);
            goto out_to;
        }

        if (write(fdTo, _buffer, n) != n) {
            fprintf(stderr, "Error: Failed to write %zd bytes.\n", n);
            goto out_to;
        }

        pos += n;
    }

    res = 0;

out_to:
    if (close(fdTo) != 0) {
        fprintf(stderr, "Error: Failed to close %s.\n", args->to);
        res = -1; // Continue clean up. There is nothing we can do about it.
    }

out_from:
    if (close(fdFrom) != 0) {
        fprintf(stderr, "Error: Failed to close %s.\n", args->from);
        res = -1; // Continue clean up. There is nothing we can do about it.
    }

    return res;
}