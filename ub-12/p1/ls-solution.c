#include "ls.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

// Use this function to display the files. DO NOT CHANGE IT.
void _printLine(unsigned int size, unsigned int sizeOnDisk, const char* name)
{
    printf("%010u   %010u   %s\n", size, sizeOnDisk, name);
}

// Assume this to be the maximum length of a file name returned by readdir
#define MAX_FILE_NAME_LENGTH 255

static int _shouldDisplay(const char *filterByExtension, const char* name)
{
    assert(name != NULL);

    // We do not display hidden files, . and ..
    if (name[0] == '.') {
        return 0;
    }

    if (filterByExtension == NULL) {
        return 1;
    }

    // Find the last dot in the file name.
    char *lastPartWithDot = strrchr(name, '.');
    if (lastPartWithDot == NULL) {
        return 0; // No extension.
    }

    // Extension starts one char after the last dot.
    char *ext = lastPartWithDot + 1;

    return (strcmp(filterByExtension, ext) == 0);
}

int list(const char* path, const char *filterByExtension)
{
    int res = -1;

    if (path == NULL) {
        return -1;
    }

    DIR *dir = opendir(path);
    if (dir == NULL) {
        return -1;
    }

    // We must build a full file name including the path for each file in the
    // path. We therefore need a buffer that is large enough to hold any
    // possible file name (+1 for delimeter and +1 for \0 char).
    size_t bufLength = strlen(path) + 1 + MAX_FILE_NAME_LENGTH + 1;
    char *fileName = (char*)malloc(bufLength);
    if (fileName == NULL) {
        goto out_close;
    }

    // A readdir error can only be detected by checking errno. However, on
    // success errno is not changed. We therefore give it a known good value
    // prior to each call.
    errno = 0;

    struct dirent *next;
    while ((next = readdir(dir)) != NULL) {
        struct stat statBuffer;

        // (1) Check if we should display the file.
        if (!_shouldDisplay(filterByExtension, next->d_name)) {
            continue;
        }

        // (2) Build the full file name.
        int r = snprintf(fileName, bufLength, "%s/%s", path, next->d_name);
        assert((size_t)r < bufLength); // Output should never be truncated...
        assert(r != -1);               // ...and format string should be ok.

        // (3) Gather information about the file and display it.
        if (stat(fileName, &statBuffer) == -1) {
            goto out_free;
        }

        _printLine(statBuffer.st_size, statBuffer.st_blocks * 512,
                   next->d_name);

        errno = 0;
    }

    // Check if the last readdir failed, or if there are no more files to list.
    // On error, errno has been modified by readdir.
    if (errno == 0) {
        res = 0;
    }

    // In C, you will often see gotos like these to ease exception handling and
    // clean up.
out_free:
    free(fileName);

out_close:
    closedir(dir);

    return res;
}
