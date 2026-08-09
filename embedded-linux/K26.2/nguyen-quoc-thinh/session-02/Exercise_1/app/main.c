#include <stdio.h>
#include "mathutils.h"

int main(void) {
    int a, b, n;

    printf("Enter two integers (for add/sub): ");
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "Invalid input.\n");
        return 1;
    }

    printf("Enter a non-negative integer (for factorial): ");
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Invalid input.\n");
        return 1;
    }

    printf("Sum: %d\n", math_add(a, b));
    printf("Difference: %d\n", math_sub(a, b));

    int fact = math_factorial(n);
    if (fact == -1) {
        printf("Factorial: invalid (n must be >= 0)\n");
    } else {
        printf("Factorial: %d\n", fact);
    }

    return 0;
}
