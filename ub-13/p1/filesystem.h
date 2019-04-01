#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <inttypes.h>

#define FILE_NAME_LENGTH 8
#define FILE_SYSTEM_BLOCKS 16
#define BLOCK_SIZE 32
#define ROOT_DIRECTORY_BLOCK 0

typedef struct {
    char data[BLOCK_SIZE];
} FileSystemBlock;

// The file system image provided with this assignment is structured as
// follows. The root directory is a file itself, which starts in block 0. You
// have to use the FAT to identify the next blocks of the root directory file.
typedef struct {
    uint32_t fat[FILE_SYSTEM_BLOCKS];   // The file allocation table (FAT)
    uint32_t rootDirectorySize;         // Root directory size in bytes.

    // The actual data blocks of the disk.
    FileSystemBlock blocks[FILE_SYSTEM_BLOCKS];
} FileSystem;

// For each file, 16 bytes (i.e., 1 block) of meta data are stored on disk in
// the root directory file using little endian byte order.
typedef struct {
    uint32_t firstBlock;                // Index of first data block.
    uint32_t length;                    // File size in bytes

    char name[FILE_NAME_LENGTH];        // File name
} DirectoryEntry;

typedef struct {
    uint32_t currentFileOffset; // Index of the next byte to read. Starts at 0.
    uint32_t currentBlock;      // Index of the current block

    uint32_t length;            // Copy of the file's length

    FileSystem *fileSystem;     // File system that owns the file
} OpenFileHandle;

FileSystem *mapFileSystem(char *diskFile);

OpenFileHandle *openFile(FileSystem *fs, char *name);
void closeFile(OpenFileHandle *handle);

int readFile(OpenFileHandle *handle, char *buffer, int length);

#endif