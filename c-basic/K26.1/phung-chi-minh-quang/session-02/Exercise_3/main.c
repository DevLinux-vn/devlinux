//Exercise_3: Chuyển đổi độ Celsius sang độ Fahrenheit
#include <stdio.h>

int main(void){
    float C, F;

    printf("Nhập độ Celsius: ");
    scanf("%f",&C);

    //Dùng phép chia 9.0f/5.0f thay vì 9/5 để C hiểu đây là phép chia số thực (float)
    F = (C * 9.0f/5.0f) + 32;

    printf("Độ Fahrenheit: %.2f\n",F);
    return 0;
}