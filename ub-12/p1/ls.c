#include "ls.h"
// You may not need all these headers ...
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Use this function to display the files. DO NOT CHANGE IT.
void _printLine(unsigned int size, unsigned int sizeOnDisk, const char* name)
{
    printf("%010u   %010u   %s\n", size, sizeOnDisk, name);
}

// Assume this to be the maximum length of a file name returned by readdir
#define MAX_FILE_NAME_LENGTH 255

int list(const char* path, const char* filterByExtension)
{
    (void)filterByExtension;
    DIR* dir = opendir(path);

    if (dir == NULL)
        return -1;

    struct dirent* curFile = readdir(dir);
    while (curFile != NULL) {
        struct stat statbuf;

        stat(curFile->d_name, &statbuf);

        _printLine(statbuf.st_size, 512 * (statbuf.st_blocks),
            curFile->d_name);
        curFile = readdir(dir);
    }

    if (closedir(dir) == -1)
        return -1;

    return 0;
}
