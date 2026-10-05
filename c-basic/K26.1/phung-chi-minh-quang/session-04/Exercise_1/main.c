// Đếm số chia hết cho 3 hoặc 5 trong khoảng từ 1 đến n

#include <stdio.h>

int main(void)
{
    int n;
    int count = 0;

    printf("Nhập n: ");
    if (scanf("%d", &n) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }

    printf("Các số chia hết cho 3 hoặc 5: ");
    for (int i = 1; i <= n; i++)
    {
        if (i % 3 == 0 || i % 5 == 0)
        {
            printf("%d ", i);
            count++;
        }
    }
    printf("\nTổng: %d số\n", count);

    return 0;
}