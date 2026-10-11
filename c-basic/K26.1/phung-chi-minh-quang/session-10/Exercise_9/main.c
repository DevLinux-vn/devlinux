// Hàm cấp phát và giải phóng bộ nhớ

// Viết chương trình C:
// 1. Viết hàm `int* createArray(int n)` để cấp phát mảy
// 2. Viết hàm `void freeArray(int *arr)` để giải phóng
// 3. Viết hàm `void print(int *arr, int n)` để in mảy
// 4. Viết hàm `void fillArray(int *arr, int n)` để nhập dữ liệu
// 5. Trong `main()`:
//    - Gọi `createArray()`
//    - Nhập dữ liệu
//    - In mảy
//    - Giải phóng

// **Ví dụ:**
// ```
// Cấp phát mảy 5 phần tử
// Nhập 5 phần tử: 1 2 3 4 5
// Mảy: 1 2 3 4 5
// Đã giải phóng an toàn
// ```
#include <stdio.h>
#include <stdlib.h>

int *createArray(int n)
{
    int *arr = malloc(n * sizeof(int));
    if (arr == NULL)
    {
        printf("Lỗi: Không đủ bộ nhớ RAM\n");
        return NULL;
    }
    return arr;
}

void freeArray(int *arr)
{
    free(arr);
    arr = NULL;
    printf("Đã giải phóng an toàn\n");
}

void print(int *arr, int n)
{
    printf("Mảng: ");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void fillArray(int *arr, int n)
{
    printf("Nhập %d phần tử: ", n);
    for (int i = 0; i < n; i++)
    {
        if (scanf("%d", &arr[i]) != 1)
        {
            printf("Lỗi: Dữ liệu nhập sai\n");
            return;
        }
    }
}

int main(void)
{
    int n;
    printf("Nhập số phần tử: ");
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

    printf("Cấp phát mảng %d phần tử: \n",n);
    int *arr = createArray(n);
    if (arr == NULL)
    {
        printf("Lỗi: Không đủ bộ nhớ RAM\n");
        return 1;
    }

    fillArray(arr, n);
    print(arr, n);

    freeArray(arr);
    return 0;
}

