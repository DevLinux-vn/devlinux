#include <stdio.h>
#include <mathutils.h>

int main() {
	int a,b;
	int n;
	printf ("Enter 2 intergers: ");
	scanf("%d %d", &a, &b);
	printf ("Enter a non-negative intger: ");
	scanf ("%d",&n);
	printf ("\n");
	printf("Addition= %d\n",math_add(a,b));
	printf("Subtraction= %d\n",math_sub(a,b));
	
	if (math_factorial(n)==-1)
		printf("Invalid argument");
	else
		printf("Factorial =%d\n",math_factorial(n));
	return 0;
}

