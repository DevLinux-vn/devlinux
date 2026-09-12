#include "mathutils.h"

/* Add two integers */
int math_add(int a, int b)
{
    return a + b;
}

/* Subtract two integers (a - b) */
int math_sub(int a, int b)
{
    return a - b;
}

/* Compute the factorial of a non-negative integer.
 * factorial(0) = 1. Returns -1 for negative input. */
int math_factorial(int n)
{
    int result = 1;
    int i;

    if (n < 0) {
        return -1;   /* invalid: factorial not defined for negatives */
    }

    for (i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}
