// Exercise_2: Tính diện tích hình chữ nhật
#include <stdio.h>

int main(void){
    float length, width;

    printf("Nhập chiều dài: ");
    scanf("%f",&length);

    printf("Nhập chiều rộng: ");
    scanf("%f",&width);

    printf("Diện tích: %.2f\n",length * width);
    return 0;
}