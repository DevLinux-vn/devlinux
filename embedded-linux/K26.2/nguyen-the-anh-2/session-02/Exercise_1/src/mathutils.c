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
    if( n >12)
    {
        printf("Overflow \n");
        return 0;
    }
    int a=1;
    int result =1;
    while(a<=n)
    {
        result *=a;
        a++;
    }
    return result;
}
