#include <stdio.h>
#include "mathutils.h"

int main() {
    int a, b, n;

    printf("Enter two integers: ");
    int ret = scanf("%d %d", &a, &b);
    if (ret != 2) {
        fprintf(stderr, "Error: Invalid input\n");
        return 1;
    }

    printf("Enter a non-negative integer for factorial: ");
    ret = scanf("%d", &n);
    if (ret != 1) {
        fprintf(stderr, "Error: Invalid input\n");
        return 1;
    }

    printf("Add: %d\n", math_add(a, b));
    printf("Subtract: %d\n", math_sub(a, b));

    int factorial = math_factorial(n);

    if (factorial == -1) {
        printf("Factorial: invalid input\n");
    } else {
        printf("Factorial: %d\n", factorial);
    }

    return 0;
}