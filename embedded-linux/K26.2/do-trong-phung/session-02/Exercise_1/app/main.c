#include <stdio.h>
#include "mathutils.h"

int main(void)
{
    int a, b, n;

    printf("Enter first integer: ");
    scanf("%d", &a);

    printf("Enter second integer: ");
    scanf("%d", &b);

    printf("Enter a non-negative integer: ");
    scanf("%d", &n);

    printf("Addition: %d\n", math_add(a, b));
    printf("Subtraction: %d\n", math_sub(a, b));
    printf("Factorial: %d\n", math_factorial(n));

    return 0;
}