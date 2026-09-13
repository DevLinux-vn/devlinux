#include <stdio.h>
#include "mathutils.h"

int main()
{
    int a, b, n;
    printf("Enter two integers (a b): ");
    if (scanf("%d %d", &a, &b) != 2) {
        printf("Invalid input for a and b\n");
        return 1;
    }


    printf("Enter a non-negative integer n: ");
    if (scanf("%d", &n) != 1 || n < 0) {
        printf("Invalid input for n\n");
        return 1;
    }

    printf("math_add: %d\r\n", math_add(a, b));
    printf("math_sub: %d\r\n", math_sub(a, b));
    int factorial = math_factorial(n);
    if(factorial < 0) {
        printf("factorial gets negative value from %d\n", n);
    } else {
        printf("factorial: %d\n", factorial);
    }
    return 0;
}
