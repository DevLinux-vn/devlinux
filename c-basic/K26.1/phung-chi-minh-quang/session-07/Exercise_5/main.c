// Tìm phần tử lớn nhất trong mảng (dùng con trỏ)
#include <stdio.h>

int *findMax(int *arr, int n)
{
    int *p_max = &arr[0];

    for (int i = 0; i < n; i++)
    {
        if (arr[i] > *p_max)
        {
            p_max = &arr[i];
        }
    }
    return p_max;
}

int main(void)
{
    int n;
    printf("Nhập số lượng phần tử: ");
    if (scanf("%d", &n) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }
    if (n <= 0)
    {
        printf("Lỗi: Vui lòng nhập số nguyên dương\n");
        return 1;
    }

    printf("Nhập %d phần tử: ", n);
    int arr[n];
    for (int i = 0; i < n; i++)
    {
        if (scanf("%d", &arr[i]) != 1)
        {
            printf("Lỗi: Dữ liệu nhập sai\n");
            return 1;
        }
    }

    int *p_max = findMax(arr, n);
    printf("Phần tử lớn nhất: %d\n", *p_max);
    printf("Địa chỉ: %p\n", (void *)p_max);

    return 0;
}