#include<stdio.h>
#include "mathutils.h"

int main(){
    int a = 5, b = 3;
    int n = 4;
    printf("Addition of %d and %d is: %d\n", a, b, math_add(a, b));
    printf("Subtraction of %d and %d is: %d\n", a, b, math_sub(a, b));
    printf("Factorial of %d is: %d\n", n, math_factorial(n));
    return 0;
}