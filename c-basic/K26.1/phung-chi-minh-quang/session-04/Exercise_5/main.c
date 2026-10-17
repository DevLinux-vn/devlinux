//In hình tam giác Pascal
#include <stdio.h>

int main(void)
{
    int n;

    printf("Nhập n: ");
    if (scanf("%d", &n) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }

    for (int i = 0; i < n; i++)
    {
        for (int space = 0; space < n - i - 1; space++)
        {
            printf("  "); // 2 khoảng trắng
        }

        int val = 0;
        for (int k = 0; k <= i; k++)
        {
            if (k == 0)
            {
                val = 1;
            }
            else
            {
                val = val * (i - k + 1) / k;
            }
            printf("%4d", val);
        }
        printf("\n");
    }

    return 0;
}