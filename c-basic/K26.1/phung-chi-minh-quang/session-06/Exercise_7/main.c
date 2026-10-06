//Tính tổng tất cả, tổng từng hàng, tổng từng cột của ma trận m x n
#include <stdio.h>

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
    printf("Ma trận:\n");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    // Sum of all elements in the matrix
    int sum_all = 0;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            sum_all += matrix[i][j];
        }
    }
    printf("Tổng tất cả: %d\n", sum_all);

    //Sum of each row
    for (int i = 0; i < m; i++)
    {
        int sum_row = 0;
        for (int j = 0; j < n; j++)
        {
            sum_row += matrix[i][j];
        }
        printf("Tổng hàng %d: %d\n", i + 1, sum_row);
    }

    //Sum of each column
    for (int j = 0; j < n; j++)
    {
        int sum_col = 0;
        for (int i = 0; i < m; i++)
        {
            sum_col += matrix[i][j];
        }
        printf("Tổng cột %d: %d\n", j + 1, sum_col);
    }

    return 0;
}