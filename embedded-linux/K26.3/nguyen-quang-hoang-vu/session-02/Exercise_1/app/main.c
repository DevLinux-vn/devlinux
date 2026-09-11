#include <stdio.h>
#include "mathutils.h"

int main(void) {
    int a, b, n;

    printf("Enter two integers (a b): ");
    scanf("%d %d", &a, &b);

    printf("Enter a non-negative integer for factorial: ");
    scanf("%d", &n);

    printf("add(%d, %d) = %d\n", a, b, math_add(a, b));
    printf("sub(%d, %d) = %d\n", a, b, math_sub(a, b));
    printf("factorial(%d) = %d\n", n, math_factorial(n));

    return 0;
}
