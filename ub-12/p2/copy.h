#ifndef COPY_H
#define COPY_H

// Size of the intermediate copy buffer. You can increase this size to improve
// copy performance if you want to.
#define BUFFER_SIZE 128

typedef struct {
    const char* from;
    const char* to;

    int skip;           // Number of bytes to skip.
    int count;          // Number of bytes to copy. -1 means infinite.
} CopyArgs;

int parseCopyArgs(int argc, char * const argv[], CopyArgs* args);

int doCopy(CopyArgs* args);

#endif