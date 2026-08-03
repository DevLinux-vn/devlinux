#include <stdio.h>
#include <errno.h>
#include "mathutils.h"

int main() {
    int a, b, n;

    printf("Enter two integers for add and subtract (e.g. 10 5): ");
    while (1) {
        int ret = scanf("%d %d", &a, &b);
        if (ret == 2) break;
        if (ret == EOF) {
            fprintf(stderr, "EOF encountered\n");
            return 1;
        }
        if (ret != EOF && errno == EINTR) {
            errno = 0;
            continue;  /* retry on EINTR */
        }
        /* clear input buffer */
        int c;
        while ((c = getchar()) != EOF && c != '\n') {}
        fprintf(stderr, "Invalid input, try again\n");
    }

    printf("Enter a non-negative integer for factorial (e.g. 5): ");
    while (1) {
        int ret = scanf("%d", &n);
        if (ret == 1) break;
        if (ret == EOF) {
            fprintf(stderr, "EOF encountered\n");
            return 1;
        }
        if (ret != EOF && errno == EINTR) {
            errno = 0;
            continue;  /* retry on EINTR */
        }
        /* clear input buffer */
        int c;
        while ((c = getchar()) != EOF && c != '\n') {}
        fprintf(stderr, "Invalid input, try again\n");
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
