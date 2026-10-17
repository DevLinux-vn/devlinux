// Đảo mảng (dùng con trỏ)
#include <stdio.h>

void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void reverse(int *arr, int n)
{
    int *left = arr;
    int *right = arr + n - 1;

    while (left < right)
    {
        swap(left, right);
        left++;
        right--;
    }
}
int main(void)
{
    int n;

    printf("Nhập số lượng phần tử: ");
    scanf("%d", &n);

    int arr[n];

    printf("Nhập %d phần tử: ", n);
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &arr[i]);
    }

    printf("Mảng ban đầu: ");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");

    reverse(arr, n);

    printf("Mảng sau đảo: ");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}