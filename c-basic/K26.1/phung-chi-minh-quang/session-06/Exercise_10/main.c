//Ma trận xoắn ốc
#include <stdio.h>

int main(void)
{
    int n;

    printf("Nhập cấp ma trận: ");
    scanf("%d", &n);

    int arr[n][n];

    printf("Nhập ma trận %dx%d\n", n, n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            scanf("%d", &arr[i][j]);
        }
    }

    printf("Ma trận:");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }

    int top = 0, bottom = n - 1, left = 0, right = n - 1;

    printf("Xoắn ốc:\n");
    while (top <= bottom && left <= right)
    {
        // 1. from left to right at the top row
        for (int i = left; i <= right; i++)
        {
            printf("%d ", arr[top][i]);
        }
        top++;

        // 2. from top to bottom at the right column
        for (int i = top; i <= bottom; i++)
        {
            printf("%d ", arr[i][right]);
        }
        right--;

        // 3. from right to left at the bottom row
        for (int i = right; i >= left; i--)
        {
            printf("%d ", arr[bottom][i]);
        }
        bottom--;

        // 4. from bottom to top at the left column
        for (int i = bottom; i >= top; i--)
        {
            printf("%d ", arr[i][left]);
        }
        left++;
    }
    printf("\n");

    return 0;
}