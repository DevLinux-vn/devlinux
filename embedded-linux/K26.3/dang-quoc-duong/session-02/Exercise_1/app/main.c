#include <stdio.h>
#include "mathutils.h"

int main(void)
{
	int a, b, n;
	
	printf("Enter two integers: ");
	scanf("%d %d", &a, &b);

	printf("Addition: %d\n", math_add(a, b));
	printf("Subtraction: %d\n", math_sub(a, b));

	printf("Enter a non-negative integer for factorial: ");
	scanf("%d", &n);

	if (n < 0)
	{
		printf("Factorial is not defined for negative integers. \n");
		return 1;
	}

	printf("Factorial: %d\n", math_factorial(n));
	return 0;
}
