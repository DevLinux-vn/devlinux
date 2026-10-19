#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int n;

    printf("Nhập số lượng phần tử ban đầu: ");
    if (scanf("%d", &n) != 1 || n <= 0)
    {
        printf("Lỗi: Vui lòng nhập số nguyên dương!\n");
        return 1;
    }

    int *arr = malloc(n * sizeof(int));
    if (arr == NULL)
    {
        printf("Lỗi: Không đủ bộ nhớ RAM\n");
        return 1;
    }
    // Nhập n phần tử ban đầu
    printf("Nhập %d phần tử: ", n);
    for (int i = 0; i < n; i++)
    {
        if (scanf("%d", &arr[i]) != 1)
        {
            printf("Lỗi: Dữ liệu nhập sai\n");
            free(arr);
            return 1;
        }
    }
    getchar();

    // Nhập số lượng phần tử mới
    int m;
    printf("Nhập số lượng phần tử mới: ");
    if (scanf("%d", &m) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        free(arr);
        return 1;
    }
    if (m <= n)
    {
        printf("Lỗi: Vui lòng nhập số lượng phần tử lớn hơn lần đầu\n");
        free(arr);
        return 1;
    }

    int *tmp= realloc(arr, m * sizeof(int)); //Lưu tạm thời phòng trường hợp realloc thất bại
    if (tmp == NULL)
    {
        printf("Lỗi: Không đủ bộ nhớ RAM\n");
        free(arr);// Giải phóng con trỏ cũ để tránh memory leak
        return 1;
    }
    arr = tmp; //gán lại địa chỉ sau khi realloc thành công
    
    printf("Nhập phần %d tử mới: ", m - n);
    for (int i = n; i < m; i++)
    {
        if (scanf("%d", &arr[i]) != 1)
        {
            printf("Lỗi: Dữ liệu nhập sai\n");
            free(arr);
            return 1;
        }
    }
    while (getchar() != '\n');

    // In mảng sau realloc
    printf("Mảng sau realloc: ");
    for (int i = 0; i < m; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Giải phóng bộ nhớ
    free(arr);

    return 0;
}