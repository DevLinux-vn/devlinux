// Xử lý mảng 2 chiều với con trỏ

// Viết chương trình C:
// 1. Viết hàm `void inputMatrix(int arr[][100], int m, int n)` để nhập ma trận `m x n`
// 2. Viết hàm `void printMatrix(int arr[][100], int m, int n)` để in ma trận
// 3. Viết hàm `int sumMatrix(int arr[][100], int m, int n)` để tính tổng tất cả phần tử
// 4. Trong `main()`:
//    - Yêu cầu nhập số dòng `m` và số cột `n`
//    - Nhập ma trận bằng hàm
//    - In ma trận
//    - Tính và in tổng

// **Ví dụ:**
// ```
// Nhập số dòng: 2
// Nhập số cột: 3
// Nhập ma trận 2x3:
// 1 2 3
// 4 5 6
// Ma trận:
// 1 2 3
// 4 5 6
// Tổng tất cả phần tử: 21
// ```

#include <stdio.h>

void inputMatrix(int arr[][100], int m, int n)
{
    printf("Nhập ma trận %dx%d:\n", m, n);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            scanf("%d", &arr[i][j]);
        }
    }
}

void printMatrix(int arr[][100], int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
}

int sumMatrix(int arr[][100], int m, int n)
{
    int total = 0;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            total = total + arr[i][j];
        }
    }
    return total;
}

int main(void){
    int m, n;
    printf("Nhập số dòng: ");
    scanf("%d", &m);
    printf("Nhập số cột: ");
    scanf("%d", &n);

    int arr[m][n];

    inputMatrix(arr, m, n);

    printMatrix(arr, m, n);

    printf("Tổng tất cả phần tử: %d\n", sumMatrix(arr, m, n));

    return 0;
}