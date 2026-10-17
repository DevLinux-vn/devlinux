#include <stdio.h>
#include "mathutils.h"

int main(void) {
    int a, b, n;

    printf("Enter two integers for add/sub (e.g., 10 5): ");
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "Error: Invalid input for add/sub.\n");
        return 1;
    }

    printf("Enter a non-negative integer for factorial: ");
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Error: Invalid input for factorial.\n");
        return 1;
    }

    printf("\n--- Results ---\n");
    printf("%d + %d = %d\n", a, b, math_add(a, b));
    printf("%d - %d = %d\n", a, b, math_sub(a, b));
    
    int fact = math_factorial(n);
    if (fact == -1) {
        printf("Factorial of %d = Error (Negative input)\n", n);
    } else {
        printf("Factorial of %d = %d\n", n, fact);
    }

    return 0;
}