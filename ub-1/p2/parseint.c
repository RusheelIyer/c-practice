#include "parseint.h"
#include <string.h>

/*
 * Returns the value of c or -1 on error
 */
int parseDecimalChar(char c)
{
    (void)c;

    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'F') {
        return (c - 'A') + 10;
    }

    return -1;
}

/*
 * Parses a non-negative integer, interpreted as octal when starting with 0,
 * decimal otherwise. Returns -1 on error.
 */
int parseInt(char *string)
{
    (void)string;

    int numChars = strlen(string);

    int multiplier = 8;
    if (!(parseDecimalChar(*string) == 0))
        multiplier = 10;

    for (int i = 0; i < numChars; i++) {
        char curChar = * (string + i);
        if (parseDecimalChar(curChar) == -1)
            return -1;
        if (multiplier == 8) {
            if (parseDecimalChar(curChar) > 7)
                return -1;
        }
    }

    int number = 0;
    int j = 0;

    for (int i = numChars - 1; i >= 0; i--) {
        int curNumber = parseDecimalChar(*(string + i));
        int power = 1;
        for (int k = 0; k < j; k++) {
            power *= multiplier;
        }

        number += (curNumber *= power);
        j++;

    }

    return number;
}
