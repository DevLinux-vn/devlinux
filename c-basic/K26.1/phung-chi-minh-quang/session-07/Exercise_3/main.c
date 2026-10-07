// Hoán đổi 2 số (swap)
#include <stdio.h>

void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}
int main(void)
{
    int a, b;

    printf("Nhập số 1: ");
    scanf("%d", &a);

    printf("Nhập số 2: ");
    scanf("%d", &b);

    printf("Trước hoán đổi: a = %d, b = %d\n", a, b);
    swap(&a, &b);
    printf("Sau hoán đổi: a = %d, b = %d\n", a, b);

    return 0;
}