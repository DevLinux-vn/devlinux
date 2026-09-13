#include "mathutils.h"

int math_add(int a, int b) {
    return a + b;
}

int math_sub(int a, int b) {
    return a - b;
}

long long math_factorial(int n) {
    if (n < 0) {
        return -1; /* Return -1 for negative numbers */
    }
    if (n == 0 || n == 1) {
        return 1; /* Factorial of 0 and 1 is 1 */
    }
    if (n > 20) {
        return -2; /* Return -2 for overflow risk (20! is max for long long) */
    }
    long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}