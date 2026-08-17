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
    if(b == 0)
    {
        printf("Khong chia duoc cho 0 \n");
        return 0.0;
    }
    else 
    {
        return a/b;
    }
}