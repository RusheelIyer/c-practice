#include "greet.h"
#include <stdio.h>

void greet(int32_t times) {
	(void) times;
	for (int i = 1; i <= times; i++) {
        printf("%d. Hello World!\n", i);
    }
}
