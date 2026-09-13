#include <stdio.h>
#include "mathutils.h"

int main(){
    int a;
    int b;
    int n;

    printf("Enter two numbers: ");
    fflush(stdout);
    scanf("%d %d", &a, &b);
    printf("Addition: %d\n", math_add(a, b));
    printf("Subtraction: %d\n", math_sub(a, b));

    printf("Enter a number to calculate factorial: ");
    fflush(stdout);
    scanf("%d", &n);
    long long factorial = math_factorial(n);
    if (factorial == -1) {
        printf("Factorial is not defined for negative numbers.\n");
    } else if (factorial == -2) {
        printf("Number %d is too large for factorial calculation (max: 20).\n", n);
    } else {        
        printf("Factorial of %d is %lld\n", n, factorial);
    }
    return 0;
}