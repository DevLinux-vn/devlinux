#include <stdio.h>
#include "mathutils.h"

int main(void)
{
    int a, b, n;

    /* Read two integers for add / subtract */
    printf("Enter two integers (a b): ");
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "Error: invalid input for a and b.\n");
        return 1;
    }

    /* Read one non-negative integer for factorial */
    printf("Enter a non-negative integer for factorial (n): ");
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Error: invalid input for n.\n");
        return 1;
    }

    printf("\n--- Results ---\n");
    printf("math_add(%d, %d)       = %d\n", a, b, math_add(a, b));
    printf("math_sub(%d, %d)       = %d\n", a, b, math_sub(a, b));

    if (n < 0) {
        printf("math_factorial(%d)    = undefined (n must be >= 0)\n", n);
    } else {
        printf("math_factorial(%d)    = %d\n", n, math_factorial(n));
    }

    return 0;
}
