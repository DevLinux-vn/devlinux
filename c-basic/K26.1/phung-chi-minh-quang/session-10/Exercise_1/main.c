// Cấp phát bộ nhớ cho Mảng số nguyên

// Viết chương trình C:
// 1. Nhập số lượng phần tử (n)
// 2. Dùng `malloc()` để cấp phát bộ nhớ cho Mảng
// 3. Kiểm tra xem `malloc()` có thành công không
// 4. Nhập n phần tử
// 5. In Mảng
// 6. Giải phóng bằng `free()`

// **Ví dụ:**
// ```
// Nhập số lượng: 3
// Nhập 3 phần tử: 5 10 15
// Mảng: 5 10 15
// Đã giải phóng bộ nhớ
#include <stdio.h>
#include <stdlib.h>

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

    int *arr = malloc(n * sizeof(int));
    if (arr == NULL)
    {
        printf("Lỗi: Không đủ bộ nhớ RAM\n");
        return 1;
    }

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

    // In mảng
    printf("Mảng: ");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Giải phóng bộ nhớ
    free(arr);
    arr = NULL;
    printf("Đã giải phóng bộ nhớ\n");
    return 0;
}