#include <stdio.h>
#include "mathutils.h"

int main()
{
    int a, b;
    int n;

    printf("Enter two integers: ");
    scanf("%d %d", &a, &b);

    printf("Enter n for factorial: ");
    scanf("%d", &n);

    printf("Add = %d\n", math_add(a, b));
    printf("Sub = %d\n", math_sub(a, b));
    printf("Factorial(%d) = %d\n", n, math_factorial(n));

    return 0;
}