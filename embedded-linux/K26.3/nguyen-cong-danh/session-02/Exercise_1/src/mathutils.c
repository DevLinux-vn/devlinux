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
    int result = 1;
    int i;

    if (n < 0)
    {
        return -1;
    }

    for (i = 1; i <= n; i++)
    {
        result *= i;
    }

    return result;
}