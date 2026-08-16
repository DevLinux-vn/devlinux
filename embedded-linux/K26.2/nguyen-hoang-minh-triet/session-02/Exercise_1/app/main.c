#include <stdio.h>
#include "mathutils.h"

int main(void)  {
    int a;
    int b;
    int n;

    printf("Enter a: ");
    scanf("%d", &a);

    printf("Enter b: ");
    scanf("%d", &b);

    printf("Enter n: ");
    scanf("%d", &n);

    printf("Add: %d\n", math_add(a, b));
    printf("Subtract: %d\n", math_sub(a, b));
    printf("Factorial: %d\n", math_factorial(n));

    return 0;
}
