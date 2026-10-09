/* app/main.c */
#include <stdio.h>
#include "mathutils.h"

int main() {
    int a, b, n;
    
    printf("Enter two integers for add/sub (e.g. 5 3): ");
    if (scanf("%d %d", &a, &b) != 2) {
        printf("Invalid input.\n");
        return 1;
    }
    
    printf("Enter a non-negative integer for factorial: ");
    if (scanf("%d", &n) != 1) {
        printf("Invalid input.\n");
        return 1;
    }
    
    printf("Result of add: %d\n", math_add(a, b));
    printf("Result of sub: %d\n", math_sub(a, b));
    printf("Result of factorial: %d\n", math_factorial(n));
    
    return 0;
}