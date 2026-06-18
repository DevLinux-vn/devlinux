#include <stdio.h>
#include "mathutils.h"

int main() {
    int a, b, n;

    printf("Enter two integers: ");
    scanf("%d %d", &a, &b);

    printf("Enter a non-negative integer for factorial: ");
    scanf("%d", &n);

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