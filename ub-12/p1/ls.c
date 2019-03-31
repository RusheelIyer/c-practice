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

<<<<<<< HEAD
// Returns 1 if the file should be shown, 0 if not
static inline int filename(const char* name, const char* extension) {

    if (name == NULL)
        return 1;

    if (extension != NULL) {

        int i = 0;

        while((name + i != NULL) && (name[i++] != 0));
        // chars after the dot.
        int charCount = 0;
        while(name[i--] != '.'){
            charCount++;
        }
        for (int j = 0; j < i; j++) {
            if ((name+j == NULL) || (extension+j == NULL))
                return 0;

            if ((name[j] != extension[j]))
                return 0;
        }

    }

    if (*name == '.')
            return 0;

    return 1;

}

=======
>>>>>>> 1362c9ca5b9d68c63c2a1bac36d4320712cd5e1c
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

<<<<<<< HEAD
        if (filename(curFile->d_name, filterByExtension) == 1)
            _printLine(statbuf.st_size, 512 * (statbuf.st_blocks),
            curFile->d_name);

=======
        _printLine(statbuf.st_size, 512 * (statbuf.st_blocks),
            curFile->d_name);
>>>>>>> 1362c9ca5b9d68c63c2a1bac36d4320712cd5e1c
        curFile = readdir(dir);
    }

    if (closedir(dir) == -1)
        return -1;

    return 0;
}
