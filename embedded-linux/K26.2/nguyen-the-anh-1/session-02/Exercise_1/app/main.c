#include <stdio.h>
#include "mathutils.h"

int main(void)
{
    int a, b, n;

    printf("Enter two integers: ");
    scanf("%d %d", &a, &b);

    printf("Enter a non-negative integer: ");
    scanf("%d", &n);

    printf("Add: %d\n", math_add(a, b));
    printf("Subtract: %d\n", math_sub(a, b));
    printf("Factorial: %d\n", math_factorial(n));

    return 0;
}
