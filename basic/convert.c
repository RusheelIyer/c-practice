#include <stdio.h>

#define LOWER 0 /* lower end of the table*/
#define UPPER 300 /* upper end of the table*/
#define STEP 20 /* step size */

main() {
	int fahr, celsius;

	fahr = UPPER;
	printf("Cel\tFahr\n");
	while (fahr >= LOWER){
		celsius = 5 * (fahr-32)/9;
		printf("%3d %6d\n", fahr, celsius);
		fahr = fahr - STEP;
	}
}
