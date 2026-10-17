//Tổng và trung bình cộng
#include <stdio.h>

int main (void){
    int n;
    int sum = 0;
    float average;

    printf("Nhập số lượng phần tử: ");
    if (scanf("%d", &n) != 1)
    {
        printf("Lỗi nhập liệu!\n");
        return 1;
    }
    if(n <= 0){
        printf("Lỗi: Vui lòng nhập số nguyên dương\n");
        return 1;
    }

    int elements[n];
    printf("Nhập %d phần tử: ", n);
    for (int i = 0; i < n; i++)
    {
        if (scanf("%d", &elements[i]) != 1)
        {
            printf("Lỗi nhập liệu!\n");
            return 1;
        }
        sum += elements[i];
    }

    average = (float)sum / n;

    printf("Tổng: %d\n", sum);
    printf("Trung bình cộng: %.2f\n", average);

    return 0;
}