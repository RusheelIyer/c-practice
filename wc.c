#include<stdio.h>

#define IN 1 //inside a word
#define OUT 0 //outside a word

main(){
	int c, nl, nw, nc, state;

	state = OUT;
	nl = nw = nc = 0;
	while ((c = getchar()) != EOF) {
		++nc;
		if (c == '\n')
			nl++;
		if (c == ' ' || c == '\n' || c == '\t')
			state = OUT;
		else if (state == OUT){
			state = IN;
			nw++;
		}
	}
	printf("the program has %d lines, %d words and %d chars\n", nl, nw, nc);
}
