#include "calc.h"
#include <math.h>   /* for NAN */

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

/* Return NAN when dividing by zero instead of crashing */
float calc_div(float a, float b)
{
    if (b == 0.0f) {
        return NAN;
    }
    return a / b;
}
