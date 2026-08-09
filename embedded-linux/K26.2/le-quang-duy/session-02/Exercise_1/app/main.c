#include <stdio.h>
#include "mathutils.h"

int main() {
    int a, b, n;
    
    printf("Nhap 2 so nguyen (a b): ");
    if (scanf("%d %d", &a, &b) != 2) return 1;
    
    printf("Nhap 1 so nguyen khong am (n): ");
    if (scanf("%d", &n) != 1 || n < 0) return 1;

    printf("\n--- Ket qua ---\n");
    printf("%d + %d = %d\n", a, b, math_add(a, b));
    printf("%d - %d = %d\n", a, b, math_sub(a, b));
    printf("%d! = %d\n", n, math_factorial(n));

    return 0;
}