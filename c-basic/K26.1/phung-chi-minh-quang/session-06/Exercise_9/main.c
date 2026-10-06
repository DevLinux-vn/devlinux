//Sắp xếp từng hàng của ma trận
#include <stdio.h>

void selection_sort(int *arr, int array_size)
{
    for (int i = 0; i < array_size - 1; i++)
    {
        int min_idx = i;
        for (int j = i + 1; j < array_size; j++)
        {
            if (arr[j] < arr[min_idx])
            {
                min_idx = j;
            }
        }

        // swap phần tử nhỏ nhất với phần đầu tiên của mảng đang duyệt
        if (min_idx != i) // chỉ swap nếu thấy phần tử nhỏ hơn ở phía sau
        {
            int tmp = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = tmp;
        }
    }
}

int main(void)
{
        int m, n; // for matrix m rows x n columns

    printf("Nhập số hàng (m): ");
    if (scanf("%d", &m) != 1)
    {
        printf("Lỗi nhập liệu!\n");
        return 1;
    }

    printf("Nhập số cột (n): ");
    if (scanf("%d", &n) != 1)
    {
        printf("Lỗi nhập liệu!\n");
        return 1;
    }

    // Check m and n are positive
    if (m <= 0 || n <= 0)
    {
        printf("Số hàng và số cột phải là số nguyên dương!\n");
        return 1;
    }

    int matrix[m][n];
    printf("Nhập ma trận:\n");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (scanf("%d", &matrix[i][j]) != 1)
            {
                printf("Lỗi nhập liệu!\n");
                return 1;
            }
        }
    }

    //Print the inputed matrix
    printf("Ma trận ban đầu:\n");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    // Sort each row of the matrix using selection sort
    for (int i = 0; i < m; i++)
    {
        selection_sort(&matrix[i][0], n);
    }

    // Print the sorted matrix
    printf("Ma trận sau khi sắp xếp:\n");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    return 0;
}