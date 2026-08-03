#include "mathutils.h"

int math_add(int a, int b) {
    return a + b;
}

int math_sub(int a, int b) {
    return a - b;
}

int math_factorial(int n) {
    if (n < 0) return 0;
    int result = 1; /* Initialize to 1 (0! = 1 and multiplicative identity) */
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}
