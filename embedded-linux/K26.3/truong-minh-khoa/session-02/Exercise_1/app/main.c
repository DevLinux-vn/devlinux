#include <stdio.h>
#include "mathutils.h"

int main()
{
    printf("math_add: %d\r\n", math_add(10, 11));
    printf("math_sub: %d\r\n", math_sub(22, 11));
    printf("math_factorial: %d\r\n",  math_factorial(10));
    return 0;
}
