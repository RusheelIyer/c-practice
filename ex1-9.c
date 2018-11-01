#include <stdio.h>

main(){
	int c;
	int lastChar = '-';

	while((c = getchar()) != EOF){
		if (c == ' '){
			if (lastChar != ' ')
				putchar(c);
		}
		else
			putchar(c);
		lastChar = c;
	}
}
