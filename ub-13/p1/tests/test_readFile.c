#include "testlib.h"
#include "filesystem.h"
#include <string.h>

int main() {
	test_start("Your readFile reads the first bytes of the file.");

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

	char buffer[100];
	test_equals_int(readFile(fd1, buffer, strlen(hello)), strlen(hello), "readFile returns the number of bytes read");
	buffer[strlen(hello)] = 0;
	test_equals_int(strcmp(buffer, hello), 0, "file data is correct");

	return test_end();
}

