#include <stdio.h>
#include <errno.h>
#include "mathutils.h"

int main() {
    int a, b, n;

    printf("Enter two integers for add and subtract (e.g. 10 5): ");
    while (1) {
        int ret = scanf("%d %d", &a, &b);
        if (ret == 2) break;
        if (errno != EINTR) {
            fprintf(stderr, "Invalid input\n");
            return 1;
        }
    }

    printf("Enter a non-negative integer for factorial (e.g. 5): ");
    while (1) {
        int ret = scanf("%d", &n);
        if (ret == 1) break;
        if (errno != EINTR) {
            fprintf(stderr, "Invalid input\n");
            return 1;
        }
    }

    if (n < 0) {
        fprintf(stderr, "Factorial undefined for negative numbers\n");
        return 1;
    }

    printf("%d + %d = %d\n", a, b, math_add(a, b));
    printf("%d - %d = %d\n", a, b, math_sub(a, b));
    printf("%d! = %d\n", n, math_factorial(n));

    return 0;
}
