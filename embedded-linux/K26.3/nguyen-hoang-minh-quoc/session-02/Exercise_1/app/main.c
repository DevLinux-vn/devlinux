#include <stdio.h>
#include "mathutils.h"

int main()
{
    int a;
    int b;
    int n;

    printf("Testing mathutils library\n");
    printf("Enter two integers: ");
    scanf("%d %d", &a, &b);

    if (a == 0 && b == 0)
    {
        printf("Please enter two integers.\n");
        return 1;
    }

    printf("Sum: %d\n", math_add(a, b));
    printf("Difference: %d\n", math_sub(a, b));

    printf("Enter an integer to calculate its factorial: ");
    scanf("%d", &n);
    printf("Factorial of %d: %d\n", n, math_factorial(n));

    return 0;
}
