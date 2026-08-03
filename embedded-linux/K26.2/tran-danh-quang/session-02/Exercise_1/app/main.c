#include <stdio.h>
#include "mathutils.h"

int main() {
    int a, b, n;

    printf("Enter two integers for add and subtract (e.g. 10 5): ");
    if (scanf("%d %d", &a, &b) != 2) return 1;

    printf("Enter a non-negative integer for factorial (e.g. 5): ");
    if (scanf("%d", &n) != 1) return 1;

    printf("%d + %d = %d\n", a, b, math_add(a, b));
    printf("%d - %d = %d\n", a, b, math_sub(a, b));
    printf("%d! = %d\n", n, math_factorial(n));

    return 0;
}
