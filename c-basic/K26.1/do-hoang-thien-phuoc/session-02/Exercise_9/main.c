#include <stdio.h>

int main() {
   int a, b;

 
    printf("Nhap so nguyen a: ");
    scanf("%d", &a);
    
    printf("Nhap so nguyen b: ");
    scanf("%d", &b);


    printf("Cong: %d + %d = %d\n", a, b, a + b);
    printf("Tru: %d - %d = %d\n", a, b, a - b);
    printf("Nhan: %d * %d = %d\n", a, b, a * b);
    printf("Chia nguyen: %d / %d = %d\n", a, b, a / b);
    
    printf("Chia du: %d %% %d = %d\n", a, b, a % b);

    return 0;
}