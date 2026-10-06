// Đếm số lần xuất hiện
#include<stdio.h>

int main(void){
    int n;
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
    }

    int x;
    printf("Nhập số cần tìm: ");
    if (scanf("%d", &x) != 1)
    {
        printf("Lỗi nhập liệu!\n");
        return 1;
    }

    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (elements[i] == x)
        {
            count++;
        }
    }

    printf("Số %d xuất hiện %d lần trong mảng\n", x, count);
    return 0;
}