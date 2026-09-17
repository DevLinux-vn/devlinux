#include <stdio.h>
#include "mathutils.h"

int main(void) {
    int a, b, n;

    printf("Enter two integers (a b): ");
    if (scanf("%d %d", &a, &b) != 2) {
        printf("Invalid input for a and b\n");
        return 1;
    }

    printf("Enter a non-negative integer n: ");
    if (scanf("%d", &n) != 1) {
        printf("Invalid input for n\n");
        return 1;
    }

    printf("%d + %d = %d\n", a, b, math_add(a, b));
    printf("%d - %d = %d\n", a, b, math_sub(a, b));

    int fact = math_factorial(n);
    if (fact < 0) {
        printf("Factorial of %d is undefined\n", n);
    } else {
        printf("%d! = %d\n", n, fact);
    }

    return 0;
}