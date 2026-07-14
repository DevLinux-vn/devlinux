#include <stdio.h>
#include "mathutils.h"

int main(void)
{
    int a;
    int b;
    int n;

    printf("Enter first integer: ");
    scanf("%d", &a);

    printf("Enter second integer: ");
    scanf("%d", &b);

    printf("\n");

    printf("Add       : %d\n", add(a, b));
    printf("Subtract  : %d\n", subtract(a, b));

    printf("\nEnter a number for factorial: ");
    scanf("%d", &n);

    printf("Factorial : %d\n", math_factorial(n));

    return 0;
}