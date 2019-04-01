#include "testlib.h"
#include "filesystem.h"
#include <string.h>

int main() {
	test_start("Your openFile returns the right file handle for a given file name.");

	FileSystem fs;
	memset(&fs, 0, sizeof(fs));

	char* hello = "Hello World\n";

	fs.rootDirectorySize = sizeof(DirectoryEntry) * 3;
	fs.fat[0] = 1;
	DirectoryEntry *dir = (DirectoryEntry*) &(fs.blocks[0]);
	dir[0].firstBlock = 2;
	dir[0].length = strlen(hello);
	strcpy(&(dir[0].name[0]), "a");
	dir[1].firstBlock = 3;
	dir[1].length = strlen(hello);
	strcpy(&(dir[1].name[0]), "fileb");
	dir[2].firstBlock = 4;
	dir[2].length = strlen(hello);
	strcpy(&(dir[2].name[0]), "filec");

	strcpy(&(fs.blocks[2].data[0]), hello);
	strcpy(&(fs.blocks[3].data[0]), hello);
	strcpy(&(fs.blocks[4].data[0]), hello);

	OpenFileHandle *fd1 = openFile(&fs, "a");
	test_equals_int64(fd1->currentBlock, 2, "a: currentBlock is correct");
	test_equals_int64(fd1->currentFileOffset, 0, "a: currentFileOffset is zero");
	test_equals_ptr(fd1->fileSystem, &fs, "a: fileSystem is correct");
	test_equals_int64(fd1->length, strlen(hello), "a: length is correct");

	OpenFileHandle *fd2 = openFile(&fs, "filec");
	test_equals_int64(fd2->currentBlock, 4, "a: currentBlock is correct");
	test_equals_int64(fd2->currentFileOffset, 0, "filec: currentFileOffset is zero");

	return test_end();
}

