/*Xếp loại theo điểm số*/
#include <stdio.h>

int main(void){
    int diem;

    printf("Nhập điểm số (0-100): ");
    if (scanf("%d", &diem) != 1) {
        printf("Lỗi: Vui lòng nhập một số nguyên.\n");
        return 1;
    }

    if (diem < 0 || diem > 100) {
        printf("Lỗi: Điểm số phải nằm trong khoảng 0-100.\n");
        return 1;
    }

    if (diem >= 90) {
        printf("Xếp loại: Xuất sắc\n");
    } else if (diem >= 80) {
        printf("Xếp loại: Giỏi\n");
    } else if (diem >= 65) {
        printf("Xếp loại: Khá\n");
    } else if (diem >= 50) {
        printf("Xếp loại: Trung bình\n");
    } else {
        printf("Xếp loại: Yếu\n");
    }

    return 0;
}