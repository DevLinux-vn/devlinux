#include "mathutils.h"


int math_add(int a, int b)
{
    return a + b;
}

int math_sub(int a, int b)
{
    return a - b;
}

int math_factorial(int n)
{
    if (n < 0) {
        return 0;
    }
    if (n == 0 || n == 1) {
        return 1; // Base case
    }
    return n * math_factorial(n - 1); // Recursive case
}
