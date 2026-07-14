#include <stdio.h>
#include "mathutils.h"

int main(void) {
    int a, b;
    printf("Enter two integers: ");
    scanf("%d %d", &a, &b);

    printf("add = %d\n", add(a, b));
    printf("sub = %d\n", sub(a, b));
    printf("factorial(%d) = %lu\n", a, factorial(a));
    return 0;
}
