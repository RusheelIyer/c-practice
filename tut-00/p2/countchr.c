#include "countchr.h"
#include <string.h>

int countchr(char *string, char c) {
	(void) string;
	(void) c;

    int numChars = strlen(string);
    int counter = 0;

    for (int i = 0; i < numChars; i++) {
        if (*(string + i) == c)
            counter++;
    }

	return counter;
}
