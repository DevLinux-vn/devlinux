#include <stdio.h>
#include "mathutils.h"

int main(void)
{
    int a, b, n;

    printf("Nhap vao 2 so nguyen: ");
    scanf("%d %d", &a, &b);

    printf("Nhap vao 1 so nguyen khong am: ");
    scanf("%d", &n);

    printf("Cong: %d\n", math_add(a, b));
    printf("Tru: %d\n", math_sub(a, b));
    printf("Giai thua: %d\n", math_factorial(n));

    return 0;
}
