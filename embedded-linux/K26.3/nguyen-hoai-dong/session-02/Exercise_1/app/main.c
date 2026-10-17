#include <stdio.h>
#include "mathutils.h"

int main(void)
{
	int a;
	int b;

	printf("Input Add function:");
	scanf("%d%d", &a, &b);
	printf("Add result: %d + %d = %d\n", a, b, math_add(a,b));

	printf("Input Sub function:");
        scanf("%d%d", &a, &b);
        printf("Sub result: %d - %d = %d\n", a, b, math_sub(a,b));

	printf("Input Factorial function:");
        scanf("%d", &a);
        printf("Factorial result: %d! = %d\n", a,  math_factorial(a));

	return 0;
}