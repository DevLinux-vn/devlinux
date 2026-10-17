#include "mathutils.h"

int math_add(int a, int b) {
    return a + b;
}

int math_sub(int a, int b) {
    return a - b;
}

int math_factorial(int n) {
    if (n < 0) {
        return -1; /* Return -1 to indicate error for negative input */
    }
    int result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}