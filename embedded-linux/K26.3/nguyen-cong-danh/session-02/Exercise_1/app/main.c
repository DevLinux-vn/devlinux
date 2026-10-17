#include <stdio.h>
#include "mathutils.h"

int main(void)
{
    int a;
    int b;
    int n;

    printf("Enter two integers: ");
    scanf("%d %d", &a, &b);

    printf("Enter a non-negative integer for factorial: ");
    scanf("%d", &n);

    printf("Addition: %d\n", math_add(a, b));
    printf("Subtraction: %d\n", math_sub(a, b));
    printf("Factorial: %d\n", math_factorial(n));

    return 0;
}