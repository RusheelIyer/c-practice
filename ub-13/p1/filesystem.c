#include "filesystem.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

FileSystem* mapFileSystem(char* diskFile)
{
    if (diskFile == NULL) {
        return NULL;
    }

    int fd = open(diskFile, O_RDONLY);
    if (fd == -1)
        return NULL;

    struct stat statbuffer;
    if (fstat(fd, &statbuffer) == -1) {
        close(fd);
        return NULL;
    }

    void* map = mmap(diskFile, statbuffer.st_size, PROT_READ, MAP_PRIVATE,
        fd, 0);
    if (map == (void*)-1) {
        close(fd);
        return NULL;
    }

    return (FileSystem*)map;
}

static OpenFileHandle* _openFileAtBlock(FileSystem* fs, uint32_t blockIndex,
    uint32_t length)
{
    assert(fs != NULL);
    assert(blockIndex < FILE_SYSTEM_BLOCKS);

    OpenFileHandle* handle = malloc(sizeof(OpenFileHandle));
    if (handle == NULL) {
        return NULL;
    }

    handle->fileSystem = fs;
    handle->currentBlock = blockIndex;
    handle->currentFileOffset = 0;
    handle->length = length;

    return handle;
}

static int _hasMoreBytes(OpenFileHandle* handle)
{
    assert(handle != NULL);
    assert(handle->currentFileOffset <= handle->length);

    if (handle->currentFileOffset < handle->length) {
        return 1;
    }

    return 0;
}

#define INVALID_BLOCK_INDEX (uint32_t) - 1

OpenFileHandle* openFile(FileSystem* fs, char* name)
{
    if ((fs == NULL) || (name == NULL)) {
        return NULL;
    }

    // Open the root directory file.
    OpenFileHandle* root = _openFileAtBlock(fs, ROOT_DIRECTORY_BLOCK,
        fs->rootDirectorySize);
    if (root == NULL) {
        return NULL;
    }

    uint32_t startBlock = INVALID_BLOCK_INDEX;
    uint32_t length;

    // We scan through the directory file to locate the requested file. With
    // each iteration, we read a single directory entry (struct DirectoryEntry).
    while (_hasMoreBytes(root)) {
        DirectoryEntry entry;

        int readBytes = readFile(root, (void*)&entry, sizeof(DirectoryEntry));
        if (readBytes != sizeof(DirectoryEntry)) {
            break; // A read error occurred. This should not happen!
        }

        // Check if this is the file that we are looking for.
        if (strcmp(entry.name, name) == 0) {
            startBlock = entry.firstBlock;
            length = entry.length;

            break;
        }
    }

    closeFile(root);

    return (startBlock != INVALID_BLOCK_INDEX) ? _openFileAtBlock(fs, startBlock, length) : NULL;
}

void closeFile(OpenFileHandle* handle)
{
    if (handle == NULL) {
        return;
    }

    // Since we opened the file system with read access only, we do not have
    // any pending modifications that might need to be written back to the file
    // prior closing. Hence, we can just free the handle and call it a day.
    free(handle);
}

static char _readFileByte(OpenFileHandle* handle)
{
    assert(handle != NULL);
    assert(_hasMoreBytes(handle));
    assert(handle->fileSystem != NULL);
    assert(handle->currentBlock < FILE_SYSTEM_BLOCKS);

    FileSystem* fs = handle->fileSystem;

    uint32_t offsetInBlock = handle->currentFileOffset % BLOCK_SIZE;
    char readByte = fs->blocks[handle->currentBlock].data[offsetInBlock];

    if (offsetInBlock == BLOCK_SIZE - 1) {
        handle->currentBlock = fs->fat[handle->currentBlock];
    }

    handle->currentFileOffset++;
    return readByte;
}

// This acts like the default linux read() system call on your file.
int readFile(OpenFileHandle* handle, char* buffer, int length)
{
    if ((handle == NULL) || (buffer == NULL)) {
        return -1;
    }

    int n = 0;
    while ((n < length) && _hasMoreBytes(handle)) {
        buffer[n] = _readFileByte(handle);

        ++n;
    }

    return n;
}
