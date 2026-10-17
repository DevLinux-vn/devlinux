#include <stdio.h>

int main() {
    int a = 5, b = 2;

    printf("a = %d, b = %d\n", a, b);


    int chia_nguyen = a / b;
    printf("Chia nguyen: %d / %d = %d\n", a, b, chia_nguyen);


    float chia_thuc = (float)a / b;
    printf("Chia thuc: (float)%d / %d = %.2f\n", a, b, chia_thuc);

    return 0;
}