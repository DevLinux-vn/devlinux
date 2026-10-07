// Đổi giá trị bằng con trỏ
#include <stdio.h>

int main(void)
{
    int a = 5;
    int *p = &a;

    printf("Giá trị a ban đầu: %d\n", a);
    *p = 20; // Thay đổi giá trị của a thông qua con trỏ p
    printf("Giá trị a sau thay đổi thông qua con trỏ p: %d\n", a);
    return 0;
}