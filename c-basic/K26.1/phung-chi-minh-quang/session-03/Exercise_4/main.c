/*Kiểm tra đủ tuổi thi bằng lái*/
#include <stdio.h>

int main(void){
    int tuoi;

    printf("Nhập tuổi: ");
    if (scanf("%d", &tuoi) != 1) {
        printf("Lỗi: Vui lòng nhập một số nguyên.\n");
        return 1;
    }

    if(tuoi < 0) {
        printf("Lỗi: Tuổi không thể là số âm.\n");
        return 1;
    }

    if (tuoi >= 18) {
        printf("Bạn đủ điều kiện thi bằng lái xe máy\n");
    } else {
        printf("Bạn không đủ điều kiện thi bằng lái xe máy\n");
    }

    return 0;
}