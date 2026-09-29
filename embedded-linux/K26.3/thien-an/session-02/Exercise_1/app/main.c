#include <stdio.h>
#include "mathutils.h"

int main()
{
    int a, b;
    printf("Enter a and b: \n");
    scanf("%d%d", &a, &b);
    printf("Add: %d\n", math_add(a, b));
    printf("Sub: %d\n", math_sub(a, b));
    printf("Factorial of a: %d\n", math_factorial(a));
    printf("Factorial of b: %d\n", math_factorial(b));
    return 0;
}