#include <stdio.h>
#include "mathutils.h"

int main() {
    int a, b;
    
    printf("=== Static Library Demo (mathutils) ===\n\n");
    
    printf("Enter first integer: ");
    scanf("%d", &a);
    
    printf("Enter second integer: ");
    scanf("%d", &b);
    
    printf("\nResults:\n");
    printf("%d + %d = %d\n", a, b, add(a, b));
    printf("%d - %d = %d\n", a, b, subtract(a, b));
    printf("factorial(%d) = %ld\n", a, factorial(a));
    printf("factorial(%d) = %ld\n", b, factorial(b));
    
    return 0;
}
