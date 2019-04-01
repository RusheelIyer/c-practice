#include "testlib.h"
#include "filesystem.h"
#include <unistd.h>
#include <stdio.h>

#define HELLO_SIZE 1

char *files[] = { "hello", "secret", "chain", "alpha", "endian", NULL };

int main()
{
    test_start("filesystem.c");
    char buf[1024];

    printf("Mapping file system...\n");
    FileSystem *fs = mapFileSystem("test.image");
    if (fs == NULL) {
        test_failed_message("No file system returned.");
    }

    // Test that you loaded the right file system.
    test_equals_int64(fs->rootDirectorySize, 5 * sizeof (DirectoryEntry), "file system size is correct");

    printf("Reading some files:\n");
    // We want to read the file "hello" and some other files.

    for (char **fileName = files; *fileName != NULL; fileName++) {
        printf("\n");
        printf("Reading file %s\n", *fileName);
        OpenFileHandle *file = openFile(fs, *fileName);

        if (file == NULL) {
            test_failed_message("File is not there.");
        } else {
            int n = readFile(file, buf, file->length);
            char msg[100]; snprintf(msg, sizeof msg, "File %s has correct length", *fileName);
            test_equals_int64(n, file->length, msg);

            // Write the file content to stdout
            printf("File contents: ");
            fflush(stdout);
            if (write(1, buf, n) < 0)
                test_failed_message("write failed");
            fflush(stdout);
            printf("<EOF>\n");
            closeFile(file);
        }
    }

    return test_end();
}
