// In địa chỉ và giá trị
#include <stdio.h>

int main(void)
{
    int x = 10;
    int *p = &x; // p là con trỏ trỏ tới x

    printf("Giá trị của x: %d\n", x);
    printf("Địa chỉ của x: %p\n", (void *)&x);
    printf("Giá trị của con trỏ p: %p\n", (void *)p);
    printf("Giá trị mà con trỏ p trỏ tới: %d\n", *p);
    return 0;
}