#include <stdio.h>
#include "mathutils.h"

int main()
{
    int a;
    int b;
    int n;

    printf("Testing mathutils library\n");
    printf("Enter two integers: ");
    if (scanf("%d %d", &a, &b) != 2)
    {
        printf("Invalid input\n");
        return 1;
    }

    printf("Sum: %d\n", math_add(a, b));
    printf("Difference: %d\n", math_sub(a, b));

    printf("Enter an integer to calculate its factorial: ");
    if (scanf("%d", &n) != 1)
    {
        printf("Invalid input\n");
        return 1;
    }

    printf("Factorial of %d: %d\n", n, math_factorial(n));

    return 0;
}
