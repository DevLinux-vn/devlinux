//Kiểm tra số hoàn hảo
#include <stdio.h>

int main(void)
{
    int n;

    printf("input n: ");
    if (scanf("%d", &n) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }

    printf("Các số hoàn hảo ≤ %d:", n);

    for (int i = 1; i <= n; i++)
    { // Xét lần lượt các số i trong khoảng từ 1 đến n
        // printf("i = %d\n",i);
        int divisor_total = 0;
        int j = 1;

        // Tìm ước cho mỗi số i
        while (j < i)
        {
            if (i % j == 0)
            {
                // j là ước của số i
                // Cộng tổng các ước của i
                divisor_total = divisor_total + j;
            }
            j++;
        }

        // Kiểm tra điều kiện số hoàn hảo
        if (divisor_total == i)
        {
            // i là số hoàn hảo
            printf(" %d", i);
        }
    }

    printf("\n");

    return 0;
}