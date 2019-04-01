#include "testlib.h"
#include "filesystem.h"
#include <string.h>
#include <stdio.h>

int main() {
	test_start("Your readFile can handle block boundaries.");

	FileSystem fs;
	memset(&fs, 0, sizeof(fs));

	char* hello = "Hello World\nThis is a pretty long text that does not fit in one file.";

	fs.rootDirectorySize = sizeof(DirectoryEntry) * 3;
	fs.fat[0] = 1;
	DirectoryEntry *dir = (DirectoryEntry*) &(fs.blocks[0]);
	dir[0].firstBlock = 2;
	dir[0].length = strlen(hello);
	fs.fat[2] = 6;
	fs.fat[3] = 4;
	fs.fat[4] = 5;
	fs.fat[6] = 10;
	strcpy(&(dir[0].name[0]), "a");
	dir[1].firstBlock = 6;
	dir[1].length = strlen(hello);
	strcpy(&(dir[1].name[0]), "fileb");
	dir[2].firstBlock = 10;
	dir[2].length = strlen(hello);
	strcpy(&(dir[2].name[0]), "filec");

	strncpy(&(fs.blocks[2].data[0]), hello, BLOCK_SIZE);
	strncpy(&(fs.blocks[6].data[0]), hello+BLOCK_SIZE, BLOCK_SIZE);
	strncpy(&(fs.blocks[10].data[0]), hello+2*BLOCK_SIZE, BLOCK_SIZE);

	OpenFileHandle *fd1 = openFile(&fs, "a");

	char buffer[1000];
	test_equals_int(readFile(fd1, buffer, strlen(hello)), strlen(hello), "readFile reads the whole file");
	buffer[strlen(hello)] = 0;
	test_equals_int(strcmp(buffer, hello), 0, "file contents are correct");

	return test_end();
}

