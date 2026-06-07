#include "mathutils.h"

int add(int a, int b)
{
    return a + b;
}

int subtract(int a, int b)
{
    return a - b;
}

unsigned long long factorial(int n)
{
    if (n < 0)
    {
        return 0;
    }

    unsigned long long result = 1;

    for (int i = 1; i <= n; i++)
    {
        result *= i;
    }

    return result;
}