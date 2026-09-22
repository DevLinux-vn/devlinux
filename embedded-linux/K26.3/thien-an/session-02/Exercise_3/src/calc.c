#include "calc.h"
#include <stdio.h>
#include <math.h>

/* include/calc.h */
float calc_add(float a, float b)
{
    return a + b;
};
float calc_sub(float a, float b)
{
    return a - b;
};
float calc_mul(float a, float b)
{
    return a * b;
};
float calc_div(float a, float b)
{
    if (b == 0.0f)
    {
        /* Error here */
        return NAN;
    }
    return ((float) a / b);
};   /* returns NAN on division by zero */