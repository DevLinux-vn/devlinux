#include <stdio.h>
#include "mathutils.h"

int main() {
    int a, b, n;

    printf("Nhap 2 so nguyen (a b): ");
    scanf("%d %d", &a, &b);
    printf("%d + %d = %d\n", a, b, math_add(a, b));
    printf("%d - %d = %d\n", a, b, math_sub(a, b));

    printf("Nhap 1 so nguyen khong am (n): ");
    scanf("%d", &n);
    if (n < 0) {
        printf("Vui long nhap so khong am!\n");
    } else {
        printf("Giai thua cua %d la: %d\n", n, math_factorial(n));
    }

    return 0;
}