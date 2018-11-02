#include <stdio.h>

void reverse(char *s);

main() {
	char s[] = "rusheel";
	reverse(s);
	return 0;
}

/* reverse: reverse and print the given text */
void reverse(char *s) {
	int arraySize = sizeof(s)/sizeof(char);
	for (int i = arraySize; i >= 0; i--)
		printf("%c", s[i]);
	printf("\n");
}	
