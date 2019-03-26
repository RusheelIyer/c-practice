#include<stdio.h>

main(){
	int c, i, nWhite, nOther;
	int nDigit[10];

	nWhite = nOther = 0;
	for (i = 0; i < 10; i++)
		nDigit[i] = 0;

	while ((c = getchar()) != EOF)
		if (c >= '0' && c <= '9')
			nDigit[c-'0']++;
		else if (c == ' ' || c == '\n' || c == '\t')
			nWhite++;
		else
			nOther++;

	printf("Digits:\n");
	for (i = 0; i < 10; i++) {
		printf("%d: %d occurrences\n", i, nDigit[i]);
	}
	printf("White Spaces: %d\nOther: %d occurrences\n", nWhite, nOther);
}
