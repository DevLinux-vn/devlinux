#include "calc.h"
#include <math.h>

float calc_add(float a, float b)
{
    return a + b;
}

float calc_sub(float a, float b)
{
    return a - b;
}

float calc_mul(float a, float b)
{
    return a * b;
}

float calc_div(float a, float b)
{
    if (b == 0)
    {
        return NAN;
    }

    return a / b;
}