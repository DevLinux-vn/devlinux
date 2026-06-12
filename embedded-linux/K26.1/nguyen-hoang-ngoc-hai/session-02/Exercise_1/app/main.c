#include <stdio.h>
#include "mathutils.h"

int main() {
    int a, b, n;

    printf("Enter two integers: ");
    scanf("%d %d", &a, &b);

    printf("Enter a non-negative integer: ");
    scanf("%d", &n);

    printf("Add: %d\n", math_add(a, b));
    printf("Subtract: %d\n", math_sub(a, b));

    int result = math_factorial(n);

    if (result == -1)
        printf("Invalid input for factorial\n");
    else
        printf("Factorial: %d\n", result);

    return 0;
}