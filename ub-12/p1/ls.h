#ifndef LS_H
#define LS_H

int list(const char* path, const char *filterByExtension);


/* Necessary for test_printLine */
__attribute__((weak))
void _printLine(unsigned int size, unsigned int sizeOnDisk, const char* name);

#endif

