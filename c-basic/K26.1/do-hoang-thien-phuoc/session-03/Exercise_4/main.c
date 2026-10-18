#include <stdio.h>



int main() {
    int tuoi;

   
    printf("Nhập tuổi: ");
    scanf("%d", &tuoi);

   
    if (tuoi >= 18) {
        printf("Bạn đủ điều kiện thi bằng lái xe máy\n");
    } else {
        printf("Bạn chưa đủ điều kiện thi bằng lái xe máy\n");
    }

    return 0;
}
    
    
