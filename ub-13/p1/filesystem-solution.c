#include "filesystem.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

FileSystem *mapFileSystem(char *diskFile)
{
    if (diskFile == NULL) {
        return NULL;
    }

    int fd = open(diskFile, O_RDONLY);
    if (fd == -1) {
        return NULL;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        goto out_error;
    }

    // We capture file system corruptions only through asserts
    assert((size_t )sb.st_size >= sizeof(FileSystem));

    // Since the file system is small enough, we just fully map it into our
    // address space. This way, we can very easily perform random access later.
    void *mem = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mem == NULL) {
        goto out_error;
    }

    return (FileSystem*)mem;

out_error:
    close(fd);
    return NULL;
}

static OpenFileHandle *_openFileAtBlock(FileSystem *fs, uint32_t blockIndex,
    uint32_t length)
{
    assert(fs != NULL);
    assert(blockIndex < FILE_SYSTEM_BLOCKS);

    OpenFileHandle *handle = malloc(sizeof(OpenFileHandle));
    if (handle == NULL) {
        return NULL;
    }

    handle->fileSystem        = fs;
    handle->currentBlock      = blockIndex;
    handle->currentFileOffset = 0;
    handle->length            = length;

    return handle;
}

static int _hasMoreBytes(OpenFileHandle *handle)
{
    assert(handle != NULL);
    assert(handle->currentFileOffset <= handle->length);

    // More bytes can be read as long as we do not have reached the end of
    // the file.
    return (handle->currentFileOffset < handle->length);
}

#define INVALID_BLOCK_INDEX (uint32_t)-1

OpenFileHandle *openFile(FileSystem *fs, char *name)
{
    if ((fs == NULL) || (name == NULL)) {
        return NULL;
    }

    // Open the root directory file.
    OpenFileHandle *root = _openFileAtBlock(fs, ROOT_DIRECTORY_BLOCK,
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
            length     = entry.length;

            break;
        }
    }

    closeFile(root);

    return (startBlock != INVALID_BLOCK_INDEX) ?
        _openFileAtBlock(fs, startBlock, length) : NULL;
}

void closeFile(OpenFileHandle *handle)
{
    if (handle == NULL) {
        return;
    }

    // Since we opened the file system with read access only, we do not have
    // any pending modifications that might need to be written back to the file
    // prior closing. Hence, we can just free the handle and call it a day.
    free(handle);
}

char _readFileByte(OpenFileHandle *handle)
{
    assert(handle != NULL);
    assert(_hasMoreBytes(handle));
    assert(handle->fileSystem != NULL);
    assert(handle->currentBlock < FILE_SYSTEM_BLOCKS);
    FileSystem *fs = handle->fileSystem;

    // Read one byte from our memory mapped file system.
    uint32_t offsetInBlock = handle->currentFileOffset % BLOCK_SIZE;
    char readByte = fs->blocks[handle->currentBlock].data[offsetInBlock];

    if (offsetInBlock == BLOCK_SIZE - 1) {
        // We read the last byte in the current block and have to advance to
        // the next block by looking it up in the FAT. Do not care if we are
        // already at the end of the file, because then the value will
        // never be used.
        handle->currentBlock = fs->fat[handle->currentBlock];
    }

    handle->currentFileOffset++;
    return readByte;
}

int readFile(OpenFileHandle *handle, char *buffer, int length)
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