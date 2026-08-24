#include "calc.h"
#include <stdio.h>
float calc_add(float a, float b)
{
    return a+b;
}
float calc_sub(float a, float b)
{
    return a-b;
}
float calc_mul(float a, float b)
{
    return a*b;
}
float calc_div(float a, float b)
{
    return (b == 0) ? 0 : (a / b);
}